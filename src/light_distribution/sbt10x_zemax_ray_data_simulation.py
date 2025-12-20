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

# Choose how to map current -> radiometric flux:
#  - "linear": flux scales linearly with current (default)
#  - "fixed": use LED_FLUX_AT_DRIVE_W exactly (e.g., from measurement)
FLUX_MODEL = "linear"
LED_FLUX_AT_DRIVE_W = 3.44  # only used when FLUX_MODEL == "fixed"


def radiometric_flux_at_drive(current_a):
    if FLUX_MODEL == "fixed":
        return float(LED_FLUX_AT_DRIVE_W)
    # linear model
    return float(LED_FLUX_AT_1A_W) * float(current_a)


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


def plot_heatmap_absolute_irradiance(x, y, flux_W, bins=200,
                                     title="Irradiance",
                                     cmap="jet",
                                     filename="irradiance_single_led_10mm.png"):
    """
    Builds a 2D histogram in absolute watts per bin, converts to irradiance (W/cm^2),
    and saves a heatmap PNG.
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
    H_W_per_cm2 = H_W_per_mm2 * 100.0  # mm^2 -> cm^2

    plt.figure(figsize=(6, 5))
    plt.imshow(
        H_W_per_cm2.T,
        origin="lower",
        extent=[xe[0], xe[-1], ye[0], ye[-1]],
        aspect="equal",
        cmap=cmap
    )
    plt.colorbar(label="Irradiance (W/cm²)")
    plt.xlabel("x (mm)")
    plt.ylabel("y (mm)")
    plt.title(title)
    plt.tight_layout()
    # plt.show()

    plt.savefig(filename, dpi=200)
    plt.close()

    print("Saved:", filename)
    print("Peak irradiance: {:.6f} W/cm²".format(np.max(H_W_per_cm2)))


if __name__ == "__main__":
    sdf_path = "SBT-10_1000mA_#1-20200526-1_175000_5000000Rays_ZEMAX.sdf"

    rays = load_sdf(sdf_path)
    print("Loaded rays:", rays.shape)

    # Convert SDF weights -> absolute watts at your drive current
    led_flux_w = radiometric_flux_at_drive(LED_CURRENT_A)
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
        filename="irradiance_single_led_10mm.png"
    )
