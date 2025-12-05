# -*- coding: utf-8 -*-
import math
import os
import struct
import numpy as np
import matplotlib.pyplot as plt

# -----------------------------
# CONFIG
# -----------------------------
DISABLE_CENTER_LED = True  # toggle this to enable/disable the center LED (ring 0)

# LED drive + radiometric power (365 nm version)
LED_CURRENT_A = 2.15              # your drive current per LED
LED_FLUX_1A_W_TYP = 1.6           # typical radiometric flux at 1 A from datasheet
# Simple assumption: flux scales ~ linearly with current.
# Change this if you want to use a different number (e.g. min 1.3 W or derated).
LED_FLUX_PER_LED_W = LED_FLUX_1A_W_TYP * LED_CURRENT_A  # ≈ 3.44 W per LED at 2.15 A


# -----------------------------
# 1) LED positions from ring config (KiCad logic)
# -----------------------------

def generate_led_positions(disable_center_led=False):
    """
    Recreate the KiCad ring placement and return a list of
    (ref, x_mm, y_mm, angle_deg) for D1..D100.
    Optionally skip the center LED (ring_index == 0).
    """

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
    flux column is initially in arbitrary units.
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

    rays = data.reshape((-1, 8))
    return rays


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
# 3) Heatmap plotting + saving (absolute irradiance)
# -----------------------------

def plot_and_save_heatmap(x, y, flux_W, distance_mm, bins=400,
                          outfile_prefix="array_irradiance",
                          window_mm=150.0,
                          center_x_mm=100.0,
                          center_y_mm=111.0):
    """
    Plot 2D heatmap cropped to a fixed physical window (e.g. 150 mm × 150 mm),
    recentered so that the center LED is at (0, 0).

    x, y are in mm in the original PCB coordinate system.
    flux_W is in watts per ray.
    Output is plotted in W/cm^2.
    """

    # Recenter: center LED goes to (0, 0)
    x_centered = x - center_x_mm
    y_centered = y - center_y_mm

    half = window_mm / 2.0

    # Fixed window: 150 mm × 150 mm around the center LED
    xmin, xmax = -half, half
    ymin, ymax = -half, half

    H_W, xe, ye = np.histogram2d(
        x_centered,
        y_centered,
        bins=bins,
        range=[[xmin, xmax], [ymin, ymax]],
        weights=flux_W
    )

    # Convert summed watts per bin -> irradiance
    # Bin size in mm
    dx = (xmax - xmin) / float(bins)
    dy = (ymax - ymin) / float(bins)
    area_mm2 = dx * dy

    # W/mm^2
    H_W_per_mm2 = H_W / area_mm2

    # W/cm^2  (1 cm^2 = 100 mm^2)
    H_W_per_cm2 = H_W_per_mm2 * 100.0

    fig, ax = plt.subplots(figsize=(7, 6))
    im = ax.imshow(
        H_W_per_cm2.T,
        origin="lower",
        extent=[xmin, xmax, ymin, ymax],
        aspect="equal",
        cmap="jet"  # blue cold, red hot
    )
    cbar = fig.colorbar(im, ax=ax)
    cbar.set_label("Irradiance (W/cm²)")
    ax.set_xlabel("x (mm) [center LED at 0]")
    ax.set_ylabel("y (mm) [center LED at 0]")

    if DISABLE_CENTER_LED:
        title = "UVEN2 mk3 100x SBT-10X (no center) at z = {} mm"
    else:
        title = "UVEN2 mk3 100x SBT-10X at z = {} mm"

    ax.set_title(title.format(distance_mm))

    # Optionally annotate max irradiance
    max_irr = np.max(H_W_per_cm2)
    ax.text(
        0.02, 0.95,
        "max = {:.3f} W/cm²".format(max_irr),
        transform=ax.transAxes,
        fontsize=10,
        bbox=dict(boxstyle="round", facecolor="white", alpha=0.7)
    )

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
    # download zemax sdf from: https://download.luminus.com/design-files/uv/SBT-10X/SBT-10X_Zx.zip
    # Path to your single LED Zemax SDF file (generated at 1 A)
    sdf_path = "SBT-10_1000mA_#1-20200526-1_175000_5000000Rays_ZEMAX.sdf"

    # 1) LED positions from the KiCad ring script
    led_positions = generate_led_positions(disable_center_led=DISABLE_CENTER_LED)
    print("Number of LEDs used in simulation:", len(led_positions))

    # 2) Load single LED ray data once (flux still in arbitrary units)
    rays = load_sdf(sdf_path)
    print("Loaded rays for single LED:", rays.shape)

    # 2a) Normalize SDF flux so sum(flux) == LED_FLUX_PER_LED_W at 2.15 A
    total_weight = np.sum(rays[:, 6])
    scale = LED_FLUX_PER_LED_W / total_weight
    rays[:, 6] *= scale  # flux column now in absolute watts for 2.15 A drive

    print("Total radiometric flux per LED set to {:.3f} W at {:.2f} A"
          .format(LED_FLUX_PER_LED_W, LED_CURRENT_A))

    # 3) Distance sweep: 10 mm to 20 mm (inclusive) in 1 mm steps
    for distance_mm in range(10, 21):
        print("Simulating distance: {} mm".format(distance_mm))

        # Propagate rays for single LED to this plane (flux is in W)
        x_single, y_single, flux_single_W = propagate_to_plane(
            rays, distance_mm=float(distance_mm)
        )

        # Replicate pattern for all LEDs
        all_x = []
        all_y = []
        all_flux_W = []

        for ref, x_led, y_led, angle_deg in led_positions:
            all_x.append(x_single + x_led)
            all_y.append(y_single + y_led)
            all_flux_W.append(flux_single_W)

        all_x = np.concatenate(all_x)
        all_y = np.concatenate(all_y)
        all_flux_W = np.concatenate(all_flux_W)

        print("  Total rays at plane: {}".format(all_x.size))

        # Plot and save PNG for this distance (absolute irradiance in W/cm^2)
        plot_and_save_heatmap(
            all_x,
            all_y,
            all_flux_W,
            distance_mm=distance_mm,
            bins=500,
            outfile_prefix="array_irradiance"
        )


if __name__ == "__main__":
    main()
