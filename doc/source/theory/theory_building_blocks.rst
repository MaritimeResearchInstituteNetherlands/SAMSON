.. _theory_building_blocks:

Building Blocks
===============

This section describes the fundamental building blocks used to perform a
SAMSON computation. The nautical area is defined first. This area
consists of traffic objects, static objects, weather stations, and
ERTVs.

Traffic objects are the links and raster cells that together represent
the nautical area. The links describe the shipping lanes for the bounded
traffic, and the raster cells cover the entire area. Ship types can be
assigned to both links and cells. Traffic objects with assigned ships
are referred to collectively as *traffic*.

Static objects are exposed to this traffic. There is a likelihood that
ships may collide with static objects due to drift or ramming events.

Weather stations provide the probability distributions of weather
conditions, which are relevant for the exposure and measure
computations.

ERTVs can be deployed as a measure in an area to reduce the number of
drift collisions.

.. toctree::
   :maxdepth: 1

   theory_area
   theory_ships
   theory_traffic