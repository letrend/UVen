# -*- coding: utf-8 -*-
import os
import struct
import numpy as np
import matplotlib.pyplot as plt


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
        raise ValueError("Header size too small — file format mismatch.")

    with open(filename, "rb") as f:
        f.seek(header_size)
        data = np.fromfile(f, dtype="<f4")

    if data.size != number_of_rays * 8:
        raise ValueError("Ray count mismatch.")

    return data.reshape((-1, 8))


def propagate_to_plane(rays, distance_mm=10.0):
    """
    Propagate rays to a plane +distance_mm in z.
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
    forward = t > 0

    x = x[forward]
    y = y[forward]
    l = l[forward]
    m = m[forward]
    flux = flux[forward]
    t = t[forward]

    x_plane = x + t * l
    y_plane = y + t * m

    return x_plane, y_plane, flux


def plot_heatmap(x, y, flux, bins=200, title="Irradiance at 10 mm"):
    margin = 0.02
    xmin, xmax = np.percentile(x, [margin * 100, (1 - margin) * 100])
    ymin, ymax = np.percentile(y, [margin * 100, (1 - margin) * 100])

    H, xe, ye = np.histogram2d(
        x, y, bins=bins, range=[[xmin, xmax], [ymin, ymax]], weights=flux
    )

    plt.figure(figsize=(6, 5))
    plt.imshow(
        H.T,
        origin="lower",
        extent=[xe[0], xe[-1], ye[0], ye[-1]],
        aspect="equal",
    )
    plt.colorbar(label="Irradiance (arb. units)")
    plt.xlabel("x (mm)")
    plt.ylabel("y (mm)")
    plt.title(title)
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    sdf_path = "SBT-10_1000mA_#1-20200526-1_175000_5000000Rays_ZEMAX.sdf"

    rays = load_sdf(sdf_path)
    print("Loaded rays:", rays.shape)

    x_plane, y_plane, flux = propagate_to_plane(rays, distance_mm=10.0)
    print("Rays intersecting plane:", len(x_plane))

    plot_heatmap(x_plane, y_plane, flux, bins=300)
