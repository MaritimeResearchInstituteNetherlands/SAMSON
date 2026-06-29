.. _`theory`:

******
Theory 
******

The SAMSON model can be used to evaluate the risks and safety of maritime traffic in a specific area. This model employs a probabilistic approach 
at a macro level to calculate the likelihood of ships colliding with other vessels or stationary objects within the area. It also estimates the 
number of collisions over a given period using historical accident data for the region. Collisions can result in structural damage to the ship and 
potential cargo tank leaks. This implementation exclusively quantifies the probability of collision occurrence, without incorporating consequence 
analysis or computing the overall risk metric. Risk can be assessed using the SAMSON output in conjunction with an additional data source that 
captures the consequences. 

The frequency of ship-ship, drift, or ram events at sea is determined by multiplying an exposure rate by a causation factor. These events can then 
be adjusted with various measures to predict the collision rate.

.. glossary::

   Exposure
      This refers to a specific traffic situation that represents a particular type of accident. It serves as an indicator for that accident.

   Causation Factor
      This is the probability that an exposure will lead to a ship-ship, drift, or ram event, derived from real accident data. More exposures 
      lead to more events.
      
   Measures
      These are actions taken to reduce the predicted number of collisions. Some measures are always active, such as the "engine repair 
      probability function" during a drift event, where the crew will attempt to regain control of the drifting vessel. Other measures, like deploying 
      a pilot, are optional and incur additional costs.

We can summarize it as follows:

.. math::

   \text{exposures} \cdot \text{causation factor} {=} \text{events}

And:

.. math::

   \text{events} \cdot \text{measures} {=} \text{collisions}     

Exposures, events, and anticipated collisions are quantified as rates over time, effectively representing their frequency. The causation factor and mitigation 
measures are expressed as probabilities between 0 and 1. In the absence of any measures, the probability of applying them becomes 1, resulting in the event 
rate being equal to the collision rate.
 
In SAMSON a schematic representation of the nautical area is constructed with :ref:`theory_building_blocks`. In this area exposures can occur between 
two ships and a ship and a static object. This concept is elaborated in :ref:`theory_exposure_types`. Hereafter the causation factors and possible measures
are discussed in section :ref:`theory_causation` and :ref:`theory_measures`. 

Table of Contents
=================

.. toctree::
    :maxdepth: 4

    theory/theory_conventions
    theory/theory_building_blocks
    theory/theory_exposure_types
    theory/theory_causation_factors
    theory/theory_measures