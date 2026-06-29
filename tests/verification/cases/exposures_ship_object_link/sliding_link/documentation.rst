Sliding objects
----------------

General
^^^^^^^

:Objective:
  Slide an object through a cell or link and validate the results
:Criteria:
  Exposure should be symmetric and similar for cell and link

One link and one object are created such that the respective area where traffic is modelled is the same for link and cell.
Two types of platforms are defined (small and large), with their centroid at the same y height as the traffic objects.
The small platform has dimensions 500x500m, the large one 2000x2000m.
For both types of platforms the x coordinate is varied between -1500 and +1500 wrt to the centroid of the link (delta_x).
This implies that when delta_x goes from -1500 to 1500:L
 - The small platform goes from not intersecting with the traffic to intersecting with the traffic to fully enclosed in the traffic and back
 - The large platform goes from not intersecting with the traffic to intersecting with the traffic to the traffic is fully enclosed within the platform and back

Links should not intersect with platforms. As can be seen from the results, this gives issues with ramCollisions.

.. figure::
    ./case1/area_overview.svg
    :alt: figure
    :align: center

    Test set-up

Input
^^^^^

.. csv-table:: weatherstations.csv
   :file: ./case1/Area/weatherstations.csv
   :widths: auto
   :header-rows: 1

.. csv-table:: windstrength.csv
   :file: ./case1/Area/windstrength.csv
   :widths: auto
   :header-rows: 1

.. csv-table:: winddirection.csv
   :file: ./case1/Area/winddirection.csv
   :widths: auto
   :header-rows: 1

.. csv-table:: shipcategories.csv
   :file: ./case1/Traffic/shipcategories.csv
   :widths: auto
   :header-rows: 1

.. csv-table:: shiplinkdata.csv
   :file: ./case1/ModelData/shiplinkdata.csv
   :widths: auto
   :header-rows: 1

.. csv-table:: shiplinks.csv
   :file: ./case1/Traffic/shiplinks.csv
   :widths: auto
   :header-rows: 1

.. csv-table:: objects.csv
   :file: ./case1/Area/objects.csv
   :widths: auto
   :header-rows: 1

Result
^^^^^^

.. _fig:Comparison:Exposures:Link:Object:Elevation:
.. figure::
    sliding_cell.svg
    :alt: Comparison:Exposures:Link:Object:Elevation
    :align: center

    Results for the cell

.. _fig:Comparison:Exposures:Link:Object:Elevation:
.. figure::
    sliding_link.svg
    :alt: Comparison:Exposures:Link:Object:Elevation
    :align: center

    Results for the link

.. literalinclude:: .check_output.txt