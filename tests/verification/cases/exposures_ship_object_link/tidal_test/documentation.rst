Tide Test
---------

General
^^^^^^^

:Objective:
  Verify that the tide is working correctly
:Criteria:
    - The drift exposure and collisions should be symmetric
    - The number of drift collisions should align with a manual approximation

In this test case, current and wind are set to 0. This enforces drift exposure to be purely based on tide.
A simple tide is defined: it has amplitude 1 and one tide period takes exactly 12 hours. The phase is set to 0.
The tide angle is set to 0, this implies that the ship can only drift east or westward.
Objects are defined in east or westward direction, see below plot for an overview of the test setup

.. figure::
    ./case1/area_overview.svg
    :alt: figure
    :align: center

    Test set-up

This relatively straightforward setup makes it relatively straightforward to calculate drift frequencies manually.
This is done in a python script and compared to the results for SAMSON.

Input
^^^^^
.. csv-table:: tide.csv
   :file: ./case1/Area/tide.csv
   :widths: auto
   :header-rows: 1

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


Result
^^^^^^

.. _fig:Comparison:Exposures:Link:Object:Elevation:
.. figure::
    figure1.svg
    :alt: Comparison:Exposures:Link:Object:Elevation
    :align: center

    Number of drift collisions versus the distance of the platform to the link

.. literalinclude:: .check_output.txt