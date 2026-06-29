.. _`theory_ships`:

Ships
=====

The ship's geometry is modeled as a rectangular prism, defined by its length, width, and height parameters. The prism 
is assigned a default velocity along its longitudinal axis, representing forward motion in the vessel's heading direction.
The vessel's vertical position within the global reference frame is defined by its draft parameter, which indicates the 
depth of the hull below the waterline. Ship types can be defined flexibly to represent either individual vessels or 
aggregated categories of ships. Modeling groups of ship types is generally more computationally efficient and practical 
for large-scale simulations or probabilistic analyses.

In summary, the following parameters are defined for each vessel model:

*  **Type** Classification or identifier of the ship, which may represent individual vessels or grouped categories.

*  **Default Speed** Preset velocity along the longitudinal axis, indicating nominal forward motion.

*  **Length** Overall longitudinal dimension of the ship's geometry.

*  **Width** Transverse dimension across the beam.

*  **Height** Vertical extent of the ship above the baseline.

*  **Draft** Depth below the waterline, specifying the vessel’s vertical position in the global reference frame.

 