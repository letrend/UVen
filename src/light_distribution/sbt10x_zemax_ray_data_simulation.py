# -*- coding: utf-8 -*-
import os
import struct
import numpy as np
import matplotlib.pyplot as plt


# -----------------------------
# ABSOLUTE POWER CONFIG
# -----------------------------
LED_CURRENT_A = 2.5

# Datasheet typical radiometric flux at 1.0 A for 365 nm is often used as the baseline.
# If you prefer guaranteed minimum bin flux at 1 A, replace 1.6 with 1.3.
LED_FLUX_AT_1A_W = 1.6

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
LED_FLUX_PER_LED_W = LED_FLUX_AT_1A_W * REL_FACTOR


def load_sdf(filename):
    """
    Load a Zemax binary .SDF spectral ray file.
    Returns an array of shape (N, 8):
    [x, y, z, l, m, n, flux_weight, wavelength]
    flux_weight is the SDF's relative ray weight (arbitrary units).
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
        raise ValueError("Header size too small - file format mismatch.")

    with open(filename, "rb") as f:
        f.seek(header_size)
        data = np.fromfile(f, dtype="<f4")

    if data.size != number_of_rays * 8:
        raise ValueError("Ray count mismatch.")

    return data.reshape((-1, 8))


def scale_flux_to_absolute_watts(rays, led_flux_w):
    """
    Scale the SDF flux weights so that sum(flux) == led_flux_w (watts).
    This converts flux weights to absolute watts per ray.
    """
    total_weight = np.sum(rays[:, 6])
    if total_weight <= 0:
        raise ValueError("Total ray weight is <= 0; cannot scale to watts.")

    scale = led_flux_w / total_weight
    rays_scaled = rays.copy()
    rays_scaled[:, 6] = rays_scaled[:, 6] * scale
    return rays_scaled


def propagate_to_plane(rays, distance_mm=10.0):
    """
    Propagate rays to a plane +distance_mm in z.
    Returns x_plane, y_plane, flux_W (watts per ray).
    """
    x, y, z, l, m, n, flux_W, wl = rays.T

    plane_z = np.mean(z) + distance_mm

    valid = np.abs(n) > 1e-9
    x = x[valid]
    y = y[valid]
    z = z[valid]
    l = l[valid]
    m = m[valid]
    n = n[valid]
    flux_W = flux_W[valid]

    t = (plane_z - z) / n
    forward = t > 0

    x = x[forward]
    y = y[forward]
    l = l[forward]
    m = m[forward]
    flux_W = flux_W[forward]
    t = t[forward]

    x_plane = x + t * l
    y_plane = y + t * m

    return x_plane, y_plane, flux_W

def aperture_average_W_per_cm2(H_W_per_cm2, xmin, xmax, ymin, ymax, radius_mm=5.0):
    """
    H_W_per_cm2: 2D array of irradiance in W/cm^2 (shape: [bins_y, bins_x] or similar)
    extent is [xmin,xmax] and [ymin,ymax] in mm, centered such that (0,0) is LED center.
    radius_mm: aperture radius (5 mm for 10 mm diameter)
    """
    ny, nx = H_W_per_cm2.shape
    xs = np.linspace(xmin, xmax, nx, endpoint=False) + (xmax - xmin) / nx / 2.0
    ys = np.linspace(ymin, ymax, ny, endpoint=False) + (ymax - ymin) / ny / 2.0
    X, Y = np.meshgrid(xs, ys)

    mask = (X**2 + Y**2) <= radius_mm**2
    return float(np.mean(H_W_per_cm2[mask]))

def plot_heatmap_absolute_irradiance(x, y, flux_W, bins=200,
                                     title="Irradiance",
                                     cmap="jet",
                                     filename="irradiance_single_led_10mm.png",
                                     aperture_radius_mm=5.0):
    """
    Builds a 2D histogram in absolute watts per bin, converts to irradiance (W/cm^2),
    computes peak irradiance and aperture-averaged meter reading,
    overlays both on the plot, and saves a PNG.
    """

    margin = 0.02
    xmin, xmax = np.percentile(x, [margin * 100, (1 - margin) * 100])
    ymin, ymax = np.percentile(y, [margin * 100, (1 - margin) * 100])

    H_W, xe, ye = np.histogram2d(
        x, y, bins=bins, range=[[xmin, xmax], [ymin, ymax]], weights=flux_W
    )

    # Convert W per bin -> irradiance
    dx_mm = (xmax - xmin) / float(bins)
    dy_mm = (ymax - ymin) / float(bins)
    bin_area_mm2 = dx_mm * dy_mm

    H_W_per_mm2 = H_W / bin_area_mm2
    H_W_per_cm2 = H_W_per_mm2 * 100.0  # mm² → cm²

    # -----------------------------
    # Peak + meter-equivalent reading
    # -----------------------------
    peak_irradiance = np.max(H_W_per_cm2)

    ny, nx = H_W_per_cm2.shape
    xs = np.linspace(xmin, xmax, nx, endpoint=False) + dx_mm / 2.0
    ys = np.linspace(ymin, ymax, ny, endpoint=False) + dy_mm / 2.0
    X, Y = np.meshgrid(xs, ys)

    mask = (X**2 + Y**2) <= aperture_radius_mm**2
    meter_irradiance = float(np.mean(H_W_per_cm2[mask]))  # W/cm²

    # -----------------------------
    # Plot
    # -----------------------------
    fig, ax = plt.subplots(figsize=(6, 5))
    im = ax.imshow(
        H_W_per_cm2.T,
        origin="lower",
        extent=[xmin, xmax, ymin, ymax],
        aspect="equal",
        cmap=cmap
    )
    cbar = fig.colorbar(im, ax=ax)
    cbar.set_label("Irradiance (W/cm²)")

    ax.set_xlabel("x (mm)")
    ax.set_ylabel("y (mm)")
    ax.set_title(title)

    # Overlay text box (top-left)
    ax.text(
        0.02, 0.98,
        "Peak: {:.3f} W/cm²\n"
        "LS128 (Ø10 mm): {:.0f} mW/cm²".format(
            peak_irradiance,
            meter_irradiance * 1000.0
        ),
        transform=ax.transAxes,
        fontsize=10,
        va="top",
        bbox=dict(boxstyle="round", facecolor="white", alpha=0.8)
    )

    plt.tight_layout()
    plt.savefig(filename, dpi=200)
    plt.close()

    print("Saved:", filename)
    print("Peak irradiance: {:.6f} W/cm²".format(peak_irradiance))
    print("Meter reading (Ø10 mm): {:.1f} mW/cm²".format(meter_irradiance * 1000.0))

    return peak_irradiance, meter_irradiance


if __name__ == "__main__":
    sdf_path = "SBT-10_1000mA_#1-20200526-1_175000_5000000Rays_ZEMAX.sdf"

    rays = load_sdf(sdf_path)
    print("Loaded rays:", rays.shape)

    # Convert SDF weights -> absolute watts at your drive current
    led_flux_w = LED_FLUX_PER_LED_W
    print("Assumed radiometric flux per LED at {:.2f} A: {:.3f} W".format(LED_CURRENT_A, led_flux_w))

    rays_W = scale_flux_to_absolute_watts(rays, led_flux_w)

    x_plane, y_plane, flux_W = propagate_to_plane(rays_W, distance_mm=10.0)
    print("Rays intersecting plane:", len(x_plane))
    print("Power reaching plane (should be close to LED flux, minus any filtered rays): {:.6f} W".format(np.sum(flux_W)))

    plot_heatmap_absolute_irradiance(
        x_plane,
        y_plane,
        flux_W,
        bins=300,
        title="Absolute irradiance at 10 mm (single LED, {:.2f} A)".format(LED_CURRENT_A),
        cmap="jet",
        filename="irradiance_single_led_10mm.png",
        aperture_radius_mm=5.0
    )
