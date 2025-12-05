# -*- coding: utf-8 -*-
import math
import os
import struct
import numpy as np
import matplotlib.pyplot as plt

# -----------------------------
# CONFIG
# -----------------------------
DISABLE_CENTER_LED = False  # <-- toggle this to enable/disable the center LED (ring 0)


# -----------------------------
# 1) LED positions from ring config (KiCad logic)
# -----------------------------

def generate_led_positions(disable_center_led=False):
    """
    Recreate the KiCad ring placement and return a list of
    (ref, x_mm, y_mm, angle_deg) for D1..D100.
    Optionally skip the center LED (ring_index == 0).
    """

    # Same as in your KiCad script
    rings = {
        0: 1,
        1: 8,
        2: 14,
        3: 20,
        4: 26,
        5: 31
    }

    ring_spacing_mm = 12.0
    center_x_mm = 100.0
    center_y_mm = 111.0

    led_positions = []
    ref_counter = 1

    for ring_index, led_count in rings.items():
        radius_mm = ring_index * ring_spacing_mm

        for i in range(led_count):
            # If we want to disable the center LED, skip ring 0 entirely
            if disable_center_led and ring_index == 0:
                # Still advance ref_counter so references match the PCB (D1 is “missing”)
                ref_counter += 1
                continue

            angle_rad = -2.0 * math.pi * i / float(led_count)
            angle_deg = -math.degrees(angle_rad)

            dx = math.cos(angle_rad)
            dy = math.sin(angle_rad)

            r_led = radius_mm
            x_led = center_x_mm + r_led * dx
            y_led = center_y_mm + r_led * dy
            ref = "D{}".format(ref_counter)

            led_positions.append((ref, x_led, y_led, angle_deg))

            ref_counter += 1

    return led_positions


# -----------------------------
# 2) Load Zemax SDF for single LED
# -----------------------------

def load_sdf(filename):
    """
    Load a Zemax binary .SDF spectral ray file.
    Returns an array of shape (N, 8):
    [x, y, z, l, m, n, flux, wavelength]
    """
    with open(filename, "rb") as f:
        header = f.read(8)
        if len(header) < 8:
            raise ValueError("File is too short to be a valid .sdf file.")

        identifier, number_of_rays = struct.unpack("<ii", header)

    bytes_per_ray = 8 * 4
    expected_data_bytes = number_of_rays * bytes_per_ray
    file_size = os.path.getsize(filename)
    header_size = file_size - expected_data_bytes

    if header_size < 8:
        raise ValueError("Header size too small; file format mismatch.")

    with open(filename, "rb") as f:
        f.seek(header_size)
        data = np.fromfile(f, dtype="<f4")

    if data.size != number_of_rays * 8:
        raise ValueError(
            "Ray count mismatch: expected {}*8 floats, got {}."
            .format(number_of_rays, data.size)
        )

    return data.reshape((-1, 8))


def propagate_to_plane(rays, distance_mm):
    """
    Propagate rays to a plane located distance_mm further in +z
    from the mean z of the rays.

    Returns:
        x_plane, y_plane, flux
    Coordinates are relative to the LED center.
    """
    x, y, z, l, m, n, flux, wl = rays.T

    plane_z = np.mean(z) + distance_mm

    valid = np.abs(n) > 1e-9
    x = x[valid]
    y = y[valid]
    z = z[valid]
    l = l[valid]
    m = m[valid]
    n = n[valid]
    flux = flux[valid]

    t = (plane_z - z) / n
    forward = t > 0.0

    x = x[forward]
    y = y[forward]
    l = l[forward]
    m = m[forward]
    flux = flux[forward]
    t = t[forward]

    x_plane = x + t * l
    y_plane = y + t * m

    return x_plane, y_plane, flux


# -----------------------------
# 3) Heatmap plotting + saving
# -----------------------------

def plot_and_save_heatmap(x, y, flux, distance_mm, bins=400,
                          outfile_prefix="array_irradiance",
                          window_mm=150.0,
                          center_x_mm=100.0,
                          center_y_mm=111.0):
    """
    Plot 2D heatmap cropped to a fixed physical window (e.g. 150 mm × 150 mm),
    recentered so that the center LED is at (0, 0).

    x, y are in mm in the original PCB coordinate system.
    center_x_mm, center_y_mm define the center LED position in that system.
    """

    # Recenter: center LED goes to (0, 0)
    x_centered = x - center_x_mm
    y_centered = y - center_y_mm

    half = window_mm / 2.0

    # Fixed window: 150 mm × 150 mm around the center LED
    xmin, xmax = -half, half
    ymin, ymax = -half, half

    H, xe, ye = np.histogram2d(
        x_centered,
        y_centered, bins=bins, range=[[xmin, xmax], [ymin, ymax]], weights=flux
    )

    fig, ax = plt.subplots(figsize=(7, 6))
    im = ax.imshow(
        H.T,
        origin="lower",
        extent=[xe[0], xe[-1], ye[0], ye[-1]],
        aspect="equal",
        cmap="jet"  
    )
    cbar = fig.colorbar(im, ax=ax)
    cbar.set_label("Irradiance (arb units)")
    ax.set_xlabel("x (mm)")
    ax.set_ylabel("y (mm)")
    if DISABLE_CENTER_LED:
        ax.set_title("UVEN2 mk3 100x SBT-10X LEDs (no center) at z = {} mm".format(distance_mm))
    else:
        ax.set_title("UVEN2 mk3 100x SBT-10X LEDs at z = {} mm".format(distance_mm))

    plt.tight_layout()

    # Save to PNG
    suffix = "no_center" if DISABLE_CENTER_LED else "all"
    filename = "{}_{}_{}mm.png".format(outfile_prefix, suffix, int(distance_mm))
    plt.savefig(filename, dpi=200)
    plt.close(fig)
    print("Saved:", filename)


# -----------------------------
# 4) Main: sweep from 10 mm to 20 mm
# -----------------------------

def main():
    # Path to your single LED Zemax SDF file
    sdf_path = "SBT-10_1000mA_#1-20200526-1_175000_5000000Rays_ZEMAX.sdf"  # change if needed

    # 1) LED positions from the KiCad ring script
    led_positions = generate_led_positions(disable_center_led=DISABLE_CENTER_LED)
    print("Number of LEDs used in simulation:", len(led_positions))

    # 2) Load single LED ray data once
    rays = load_sdf(sdf_path)
    print("Loaded rays for single LED:", rays.shape)

    # 3) Distance sweep: 10 mm to 20 mm (inclusive) in 1 mm steps
    for distance_mm in range(10, 21):
        print("Simulating distance: {} mm".format(distance_mm))

        # Propagate rays for single LED to this plane
        x_single, y_single, flux_single = propagate_to_plane(
            rays, distance_mm=float(distance_mm)
        )

        # Replicate pattern for all LEDs
        all_x = []
        all_y = []
        all_flux = []

        for ref, x_led, y_led, angle_deg in led_positions:
            all_x.append(x_single + x_led)
            all_y.append(y_single + y_led)
            all_flux.append(flux_single)

        all_x = np.concatenate(all_x)
        all_y = np.concatenate(all_y)
        all_flux = np.concatenate(all_flux)

        print("  Total rays at plane: {}".format(all_x.size))

        # Plot and save PNG for this distance
        plot_and_save_heatmap(
            all_x,
            all_y,
            all_flux,
            distance_mm=distance_mm,
            bins=500,
            outfile_prefix="array_irradiance"
        )


if __name__ == "__main__":
    main()
