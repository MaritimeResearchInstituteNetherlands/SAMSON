.. _theory_conventions:

Conventions
===========

This section describes the applicable conventions in this theory
documentation and the source code. In the user input and output, other
conventions and units may apply for practical reasons. These are then
clearly described in the user manual. An ENU-system (East–North–Up) sign
convention and SI units are applied to minimize conversions in the code
and reduce the risk of associated errors.

Reference Frames
----------------

Ship position, heading, and static-object locations on a nautical map
are presented in a geographic coordinate system
(`WGS‑84 <https://en.wikipedia.org/wiki/World_Geodetic_System>`_). A
conversion to a Cartesian coordinate system is required for exposure
computations. The exposure models are based on a 2D model with three
degrees of freedom (3DoF), denoted as :math:`x`, :math:`y`, and
:math:`\psi`. In grounding calculations, one could argue for a 2.5D
approach, since ship draft and local water depth are taken into account.

A right-handed Cartesian coordinate system is used. A distinction is
made between the Global Coordinate System (GCS) and the Local Coordinate
System (LCS). The GCS represents the world coordinate system, while the
LCS represents the coordinate system of objects such as ships, cells,
and links.

Global frame of reference
~~~~~~~~~~~~~~~~~~~~~~~~~

The ENU-system (East–North–Up) sign convention is used. The x-axis
points east, the y-axis points north, and the z-axis points upwards.
Wave, current, and wind directions are specified as *going‑to* in the
global coordinate frame. The environment can be represented as a vector,
progressing in the specified direction.

A summary of key environmental directions:

- A direction of 0 rad (0°) points towards positive x (GCS).
- A direction of :math:`\frac{\pi}{2}` rad (90°) points towards
  positive y (GCS).
- A direction of :math:`\pi` rad (180°) points towards negative x (GCS).
- A direction of :math:`\frac{3\pi}{2}` rad (270°) points towards
  negative y (GCS).

Local frame of reference
~~~~~~~~~~~~~~~~~~~~~~~~

Each ship has its own Local Coordinate System (LCS). The origin is
defined at the midpoint of the ship’s length and width.

- The x-axis points towards the bow.
- The y-axis points towards port side.
- The z-axis points upwards.

The applicable conventions for links and cells are described in the
:ref:`theory_area` section.

Units
-----

Units follow the International System of Units (SI). These units are
used throughout all algorithms, as well as in all input and output
properties.

:numref:`tab:Base-SI-units` lists the base SI units.

.. _tab:Base-SI-units:
.. table:: Base SI units

    ============= =========== =========
    Quantity name Unit symbol Unit name
    ============= =========== =========
    Length        :math:`m`   Metre
    Mass          :math:`kg`  Kilogram
    Time          :math:`s`   Second
    ============= =========== =========

The derived SI units are listed in :numref:`tab:Derived-SI-units`.

.. _tab:Derived-SI-units:
.. table:: Derived SI units

    ================== ============== ========= ==========================
    Quantity name      Unit symbol    Unit name SI base units
    ================== ============== ========= ==========================
    Angle              :math:`rad`    Radian    :math:`m\ m^{-1}`
    Energy, work, heat :math:`J`      Joule     :math:`kg\ m^{2}\ s^{-2}`
    ================== ============== ========= ==========================

Constants
---------

The applicable constants, including their SI units and values, are
listed in :numref:`tab:Constants`.

.. _tab:Constants:
.. table:: Constants

    ============================== ================== ========
    Constant                       Units              Value
    ============================== ================== ========
    Gravitational acceleration     :math:`m\ s^{-2}`    -9.813
    Water density                  :math:`kg\ m^{-3}`  1025.0
    Air density                    :math:`kg\ m^{-3}`     1.23
    Lateral hull drag coefficient  :math:`-`              0.80
    Lateral wind drag coefficient  :math:`-`              0.90
    ============================== ================== ========
