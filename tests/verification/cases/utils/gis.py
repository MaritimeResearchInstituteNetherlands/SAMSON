import numpy as np

def haversine(coord0, coord1):
    """
    Compute geodesic distance using the haversine formula.

    Parameters
    ----------
    coord0 : tuple(float, float)
        (latitude, longitude) in WGS84
    coord1 : tuple(float, float)
        (latitude, longitude) in WGS84

    Returns
    -------
    float
        Distance [m]
    """
    lat0, lon0 = coord0
    lat1, lon1 = coord1

    r_earth = 6_371_000 # [m]

    phi0 = np.radians(lat0)
    phi1 = np.radians(lat1)

    delta_phi = np.radians(lat1 - lat0)
    delta_lambda = np.radians(lon1 - lon0)

    a = (
        np.sin(delta_phi / 2.0) ** 2
        + np.cos(phi0) * np.cos(phi1)
        * np.sin(delta_lambda / 2.0) ** 2
    )
    c = 2 * np.arctan2(np.sqrt(a), np.sqrt(1 - a))

    return r_earth * c