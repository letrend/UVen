# -*- coding: utf-8 -*-
import math
import os
import struct
import numpy as np
import matplotlib.pyplot as plt

# -----------------------------
# CONFIG
# -----------------------------
DISABLE_CENTER_LED = False  # toggle this to enable/disable the center LED (ring 0)

# LED drive + radiometric power (365 nm version)
LED_CURRENT_A = 2.5              # <-- UPDATED: drive current per LED
LED_FLUX_1A_W = 1.6           # typical radiometric flux at 1 A (365 nm family)

# LS128-like meter aperture (10 mm diameter)
METER_DIAMETER_MM = 10.0
METER_RADIUS_MM = METER_DIAMETER_MM / 2.0

# Plot window (center LED at 0,0)
WINDOW_MM = 150.0


def relative_flux_factor_365nm(current_a):
    """
    Relative radiometric flux vs current for SBT-10X 365 nm,
    extracted from datasheet curve (Tc=25C, pulsed).
    Returns φ / φ(1A).
    """
    # Datasheet-derived anchor points
    currents = np.array([1.0, 1.5, 2.0, 2.5, 3.0, 4.0])
    rel_flux = np.array([1.0, 1.55, 1.95, 2.4, 2.7, 3.4])

    # Clamp outside range
    if current_a <= currents[0]:
        return rel_flux[0]
    if current_a >= currents[-1]:
        return rel_flux[-1]

    # Linear interpolation between points
    return float(np.interp(current_a, currents, rel_flux))

REL_FACTOR = relative_flux_factor_365nm(LED_CURRENT_A)
LED_FLUX_PER_LED_W = LED_FLUX_1A_W * REL_FACTOR

# -----------------------------
# 1) LED positions from ring config (KiCad logic)
# -----------------------------

def generate_led_positions(disable_center_led=False):
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
            if disable_center_led and ring_index == 0:
                ref_counter += 1
                continue

            angle_rad = -2.0 * math.pi * i / float(led_count)
            angle_deg = -math.degrees(angle_rad)

            dx = math.cos(angle_rad)
            dy = math.sin(angle_rad)

            x_led = center_x_mm + radius_mm * dx
            y_led = center_y_mm + radius_mm * dy
            ref = "D{}".format(ref_counter)

            led_positions.append((ref, x_led, y_led, angle_deg))
            ref_counter += 1

    return led_positions


# -----------------------------
# 2) Load Zemax SDF for single LED
# -----------------------------

def load_sdf(filename):
    """
    Returns array (N,8): [x,y,z,l,m,n,flux,wavelength]
    flux is initially in arbitrary units.
    """
    with open(filename, "rb") as f:
        header = f.read(8)
        if len(header) < 8:
            raise ValueError("File too short to be valid .sdf")
        identifier, number_of_rays = struct.unpack("<ii", header)

    bytes_per_ray = 8 * 4
    file_size = os.path.getsize(filename)
    expected_data_bytes = number_of_rays * bytes_per_ray
    header_size = file_size - expected_data_bytes

    if header_size < 8:
        raise ValueError("Header size too small; file format mismatch.")

    with open(filename, "rb") as f:
        f.seek(header_size)
        data = np.fromfile(f, dtype="<f4")

    if data.size != number_of_rays * 8:
        raise ValueError("Ray count mismatch.")

    return data.reshape((-1, 8))


def propagate_to_plane(rays, distance_mm):
    """
    Propagate rays to plane at mean(z)+distance_mm.
    Returns x_plane, y_plane, flux (same units as rays[:,6]).
    """
    x, y, z, l, m, n, flux, wl = rays.T
    plane_z = np.mean(z) + distance_mm

    valid = np.abs(n) > 1e-9
    x, y, z, l, m, n, flux = x[valid], y[valid], z[valid], l[valid], m[valid], n[valid], flux[valid]

    t = (plane_z - z) / n
    forward = t > 0.0

    x, y, l, m, flux, t = x[forward], y[forward], l[forward], m[forward], flux[forward], t[forward]

    x_plane = x + t * l
    y_plane = y + t * m
    return x_plane, y_plane, flux


# -----------------------------
# 3) LS128-like scanned reading map (disk average via FFT convolution)
# -----------------------------

def disk_kernel(radius_mm, dx_mm, dy_mm):
    """
    Create a normalized circular averaging kernel in pixel space.
    """
    # choose kernel half-size in pixels
    rx = int(np.ceil(radius_mm / dx_mm))
    ry = int(np.ceil(radius_mm / dy_mm))
    xs = (np.arange(-rx, rx + 1) * dx_mm)
    ys = (np.arange(-ry, ry + 1) * dy_mm)
    X, Y = np.meshgrid(xs, ys)
    K = ((X * X + Y * Y) <= (radius_mm * radius_mm)).astype(np.float64)

    s = np.sum(K)
    if s <= 0:
        raise ValueError("Kernel sum is zero; check radius and grid spacing.")
    K /= s
    return K


def fft_convolve2d_same(image, kernel):
    """
    2D convolution using FFT, returning same-size output.
    Assumes kernel is smaller than image.
    """
    H, W = image.shape
    kh, kw = kernel.shape

    # FFT size
    fh = H + kh - 1
    fw = W + kw - 1

    F_img = np.fft.rfft2(image, s=(fh, fw))
    F_ker = np.fft.rfft2(kernel, s=(fh, fw))
    conv_full = np.fft.irfft2(F_img * F_ker, s=(fh, fw))

    # Crop to "same"
    y0 = (kh - 1) // 2
    x0 = (kw - 1) // 2
    return conv_full[y0:y0 + H, x0:x0 + W]


# -----------------------------
# 4) Heatmap plotting + saving (two-panel: irradiance and LS128 scan map)
# -----------------------------

def plot_and_save_heatmaps_two_panel(x, y, flux_W, distance_mm, bins=500,
                                     outfile_prefix="array_irradiance",
                                     window_mm=150.0,
                                     center_x_mm=100.0,
                                     center_y_mm=111.0,
                                     meter_radius_mm=5.0):
    """
    Left: irradiance (W/cm^2)
    Right: LS128-equivalent scanned reading (disk-averaged irradiance, W/cm^2)
    Both over fixed window and centered so center LED is at (0,0).
    """

    # Recenter: center LED -> (0,0)
    x_centered = x - center_x_mm
    y_centered = y - center_y_mm

    half = window_mm / 2.0
    xmin, xmax = -half, half
    ymin, ymax = -half, half

    # Histogram in watts per bin
    H_W, _, _ = np.histogram2d(
        x_centered, y_centered,
        bins=bins,
        range=[[xmin, xmax], [ymin, ymax]],
        weights=flux_W
    )

    # Convert to irradiance
    dx = (xmax - xmin) / float(bins)
    dy = (ymax - ymin) / float(bins)
    area_mm2 = dx * dy

    H_W_per_mm2 = H_W / area_mm2
    H_W_per_cm2 = H_W_per_mm2 * 100.0  # W/cm^2

    # LS128 scanned reading map: disk-average of irradiance map
    K = disk_kernel(radius_mm=meter_radius_mm, dx_mm=dx, dy_mm=dy)
    H_meter_W_per_cm2 = fft_convolve2d_same(H_W_per_cm2, K)

    # Stats
    peak_irr = float(np.max(H_W_per_cm2))
    peak_meter = float(np.max(H_meter_W_per_cm2))

    # Plot two panels
    fig, axes = plt.subplots(1, 2, figsize=(13, 6))

    im0 = axes[0].imshow(
        H_W_per_cm2.T,
        origin="lower",
        extent=[xmin, xmax, ymin, ymax],
        aspect="equal",
        cmap="jet"
    )
    axes[0].set_title("Simulated irradiance (SBT-10X array)\nz = {} mm".format(distance_mm))
    axes[0].set_xlabel("x (mm) [center LED at 0]")
    axes[0].set_ylabel("y (mm) [center LED at 0]")
    axes[0].text(
        0.02, 0.98,
        "peak = {:.3f} W/cm²".format(peak_irr),
        transform=axes[0].transAxes,
        va="top",
        fontsize=10,
        bbox=dict(boxstyle="round", facecolor="white", alpha=0.8)
    )
    fig.colorbar(im0, ax=axes[0], fraction=0.046, pad=0.04, label="W/cm²")

    im1 = axes[1].imshow(
        H_meter_W_per_cm2.T,
        origin="lower",
        extent=[xmin, xmax, ymin, ymax],
        aspect="equal",
        cmap="jet"
    )
    axes[1].set_title("Simulated LS128 scan map (Ø{} mm aperture)\nz = {} mm".format(int(meter_radius_mm * 2), distance_mm))
    axes[1].set_xlabel("x (mm) [center LED at 0]")
    axes[1].set_ylabel("y (mm) [center LED at 0]")
    axes[1].text(
        0.02, 0.98,
        "peak = {:.0f} mW/cm²".format(peak_meter * 1000.0),
        transform=axes[1].transAxes,
        va="top",
        fontsize=10,
        bbox=dict(boxstyle="round", facecolor="white", alpha=0.8)
    )
    fig.colorbar(im1, ax=axes[1], fraction=0.046, pad=0.04, label="W/cm²")

    # Overall title (include current/power assumptions)
    suffix = "no_center" if DISABLE_CENTER_LED else "all"
    fig.suptitle(
        "UVEN2 mk3 | {} LEDs | {:.2f} A each\n"
        .format(len(generate_led_positions(DISABLE_CENTER_LED)), LED_CURRENT_A, LED_FLUX_PER_LED_W),
        y=0.98
    )

    plt.tight_layout(rect=[0, 0, 1, 0.95])

    filename = "{}_{}_{}mm_twopanel.png".format(outfile_prefix, suffix, int(distance_mm))
    plt.savefig(filename, dpi=200)
    plt.close(fig)

    print("Saved:", filename)
    print("  Peak irradiance: {:.6f} W/cm^2".format(peak_irr))
    print("  Peak LS128 scan reading (Ø10 mm): {:.3f} W/cm^2 ({:.0f} mW/cm^2)".format(
        peak_meter, peak_meter * 1000.0
    ))


# -----------------------------
# 5) Main: sweep from 10 mm to 20 mm
# -----------------------------

def main():
    # sdf_path = "SBT-10_1000mA_#1-20200526-1_175000_100000Rays_ZEMAX.sdf"
    # sdf_path = "SBT-10_1000mA_#1-20200526-1_175000_1000000Rays_ZEMAX.sdf"
    sdf_path = "SBT-10_1000mA_#1-20200526-1_175000_5000000Rays_ZEMAX.sdf"

    led_positions = generate_led_positions(disable_center_led=DISABLE_CENTER_LED)
    print("Number of LEDs used in simulation:", len(led_positions))

    rays = load_sdf(sdf_path)
    print("Loaded rays for single LED:", rays.shape)

    # Scale SDF ray weights to absolute watts per LED at 2.5 A (linear model)
    total_weight = np.sum(rays[:, 6])
    if total_weight <= 0:
        raise ValueError("Total ray weight <= 0; cannot scale to watts.")
    scale = LED_FLUX_PER_LED_W / total_weight
    rays[:, 6] *= scale
    print("Radiometric flux per LED set to {:.3f} W at {:.2f} A".format(LED_FLUX_PER_LED_W, LED_CURRENT_A))

    for distance_mm in range(10, 21):
        print("Simulating distance: {} mm".format(distance_mm))

        x_single, y_single, flux_single_W = propagate_to_plane(rays, distance_mm=float(distance_mm))

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

        print("  Total rays at plane:", all_x.size)

        plot_and_save_heatmaps_two_panel(
            all_x, all_y, all_flux_W,
            distance_mm=distance_mm,
            bins=500,
            outfile_prefix="array_irradiance",
            window_mm=WINDOW_MM,
            center_x_mm=100.0,
            center_y_mm=111.0,
            meter_radius_mm=METER_RADIUS_MM
        )


if __name__ == "__main__":
    main()
