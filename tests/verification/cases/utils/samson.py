import numpy as np
import pandas as pd
import shapely

from scipy.stats import norm

# Constants
G = 9.813  # Magitude of the gravitational acceleration
RHO_AIR = 1.23  # Air density
RHO_WATER = 1025  # Sea water density

CD_WIND = 0.9  # Lateral wind resistance coefficient (ship)
CD_WATER = 0.8  # Lateral resistance coefficient (underwater body)

ENVIRONMENT = {"wind_waves": [{"beaufort": 0, "T_p": 0.001, "v_b": 0.001, "z_b": 0.000, "anchor_failure": 0.010},
                              {"beaufort": 1, "T_p": 0.780, "v_b": 1.030, "z_b": 0.100, "anchor_failure": 0.010},
                              {"beaufort": 2, "T_p": 1.870, "v_b": 2.570, "z_b": 0.300, "anchor_failure": 0.010},
                              {"beaufort": 3, "T_p": 3.060, "v_b": 4.370, "z_b": 0.500, "anchor_failure": 0.010},
                              {"beaufort": 4, "T_p": 4.620, "v_b": 6.940, "z_b": 1.000, "anchor_failure": 0.035},
                              {"beaufort": 5, "T_p": 6.210, "v_b": 9.770, "z_b": 1.500, "anchor_failure": 0.070},
                              {"beaufort": 6, "T_p": 7.700, "v_b": 12.590, "z_b": 2.000, "anchor_failure": 0.126},
                              {"beaufort": 7, "T_p": 9.240, "v_b": 15.680, "z_b": 2.500, "anchor_failure": 0.210},
                              {"beaufort": 8, "T_p": 10.810, "v_b": 19.020, "z_b": 3.000, "anchor_failure": 0.350},
                              {"beaufort": 9, "T_p": 12.440, "v_b": 22.620, "z_b": 3.500, "anchor_failure": 0.490},
                              {"beaufort": 10, "T_p": 14.090, "v_b": 26.470, "z_b": 4.000, "anchor_failure": 0.630},
                              {"beaufort": 11, "T_p": 15.790, "v_b": 30.580, "z_b": 4.500, "anchor_failure": 0.700},
                              {"beaufort": 12, "T_p": float('NaN'), "v_b": float('NaN'), "z_b": float('NaN'),
                               "anchor_failure": 1.000}, ]}

# Build dataframe
df = pd.DataFrame.from_dict(ENVIRONMENT["wind_waves"])

# Derived quantities
df["omega"] = 2 * np.pi / df["T_p"]
df["wave_number"] = 4 * np.pi ** 2 / (df["T_p"] ** 2 * G)


def drift_coefficient(k_t):
    """
    Calculate drift coefficients to facilitate the drift speed function.

    Parameters
    ----------
    k_t : np.ndarray
        Coefficient [-]
    Returns
    -------
    np.ndarray
        Wave drift coefficient [-]
    """
    a = -1.4736
    b = 2.4765
    c = -0.0315

    return a * k_t ** 3 + b * k_t ** 2 + c * k_t


def drift_speed(bft_scale, draft, height):
    """
    Calculate drift speed contribution due to wind.

    Parameters
    ----------
    bft_scale : int
        Beaufort number 
    draft : np.ndarray
        Ship's draft [m]
    height : np.ndarray
        Ship's height [m], where "height - draft" corresponds with the exposed windage height.

    Returns
    -------
    np.ndarray
        Wind and waves induced drift speed [m/s].
    """
    row = df.loc[df["beaufort"] == bft_scale].iloc[0]

    omega = row["omega"]
    v_b = row["v_b"]
    z_b = row["z_b"]

    rkt = omega ** 2 / G * draft
    r = np.where(rkt <= 1, drift_coefficient(rkt), 1)

    t1 = (RHO_AIR * (height - draft) * CD_WIND / (RHO_WATER * draft * CD_WATER) * v_b ** 2)
    t3 = z_b ** 2 * G * r ** 2 / (8 * draft * CD_WATER)

    speed = np.sqrt(t1 + t3)

    return speed


def engine_repair(time):
    """
    Calculate probability that a collision between a ship and a static object will take place.

    Parameters
    ----------
    time : np.ndarray
        Delta t [s] between leaving the traffic object as a drifting ship and hitting the static object.
    Returns
    -------
    np.ndarray
        Probability [-]
    """
    probability = np.clip(3600 / (1.5 * (time - 900) + 3600), 0, 1)

    return probability


def anchor(bft_scale):
    """
    Calculate probability that a ram collision cannot be avoided.

    Parameters
    ----------
    distance : np.ndarray
        Distance [m] between leaving the traffic object as a ramming ship and hitting the static object.
    ship_length: np.ndarray
        Ship's length [m]    
    Returns
    -------
    np.ndarray
        Probability [-]
    """
    row = df.loc[df["beaufort"] == bft_scale].iloc[0]

    probability = row["anchor_failure"]

    return probability


def avoidance(distance, length):
    """
    Calculate probability that a ram collision cannot be avoided.

    Parameters
    ----------
    distance : np.ndarray
        Distance [m] between leaving the traffic object as a ramming ship and hitting the static object.
    ship_length: np.ndarray
        Ship's length [m]    
    Returns
    -------
    np.ndarray
        Probability [-]
    """
    a = 0.1
    probability = np.exp(-a * distance / length)

    return probability


def azimuth(line):
    x1, y1 = line.coords[0]
    x2, y2 = line.coords[-1]

    angle = np.arctan2(y2 - y1, x2 - x1)
    return angle  # radians


def ram_probability(ship_link, ship_speed, ship_length, ram_factor, n_ships, platform_selection, n_angles=300,
                    n_points=200, minimum_passing_distance=None):
    """
    Compute ram exposure, event frequency, and collision frequency for ship–object interactions.

    The function samples starting points along a shipping link and propagates
    stochastic ram trajectories based on a normal distribution of heading deviations.
    It then evaluates which trajectories intersect with offshore objects.

    Parameters
    ----------
    ship_link : shapely.geometry.LineString
        Geometry representing the ship route.
    ship_speed : float
        Ship speed [m/s].
    ship_length: float
        Length of the ship [m]
    ram_factor : float
        Failure probability factor (probability of loss of control).
    n_ships : float
        Traffic intensity (ships per unit time).
    platform_selection : GeoDataFrame or similar
        Collection of object geometries to test against.
    n_angles : int, optional
        Number of sampled heading deviations (default: 300).
    n_points : int, optional
        Number of start points along the link (default: 200).

    Returns
    -------
    exposure : ndarray
        Exposure per object (expected encounters).
    events : ndarray
        Expected number of drifting/ramming events.
    collisions : ndarray
        Estimated collision frequency per object.

    Notes
    -----
    - Ram trajectories are generated over a 24-hour drift distance.
    - Heading deviations follow a normal distribution (σ ≈ 0.2529 rad).
    - Collision probability combines geometric intersection and avoidance.
    """
    link_direction = azimuth(ship_link)
    ram_distance = ship_speed * 24 * 3600

    # sample start points
    start_points = shapely.line_interpolate_point(ship_link, np.linspace(0, 1, n_points), normalized=True)
    start_coords = shapely.get_coordinates(start_points)

    # sample headings
    q = (np.arange(n_angles) + 0.5) / n_angles
    angles = norm.ppf(q, loc=0, scale=0.2529)[:, None] + link_direction

    # build trajectories
    vectors = ram_distance * np.stack([np.cos(angles), np.sin(angles)], axis=2)
    trajectories = shapely.linestrings(np.stack([start_coords + vectors * 0, start_coords + vectors], axis=2))

    objects = np.asarray(platform_selection.geometry.values)

    # intersections
    intersects = shapely.intersects(trajectories, objects.reshape(-1, 1, 1))

    # avoidance
    distances = shapely.distance(start_points, objects.reshape(-1, 1))
    if minimum_passing_distance:
        hit_prob = avoidance(np.clip(distances, minimum_passing_distance, 10 ** 10), ship_length)
    else:
        hit_prob = avoidance(distances, ship_length)

    # outputs
    exposure = intersects.mean(axis=(1, 2)) * n_ships * ship_link.length
    events = exposure * ram_factor
    collisions = ((intersects.mean(axis=1) * hit_prob).mean(axis=1) * ship_link.length * ram_factor * n_ships)

    return exposure, events, collisions
