Verifying ram and drift direction probabilities
----------------

General
^^^^^^^

:Objective:
  Verify that the drift/ram directions are calculated correctly within SAMSON
:Criteria:
  - For ram, objects at the same distance from the cell should have the same exposure
  - For drift, the drift direction should be calculated correctly

In this test case, a single cell of 200mx200m is created.
This ensures that internally the cell is discretized using one point at the center of the cell.
A circle with a radius of 2000m is drawn around the center of the cell. This circle is approximated by a regular multipolygon with 360 segments.
These 360 segments are used as objects for calculating the ram/drift exposure.
This method ensures that the exposure corresponding to the object approximates the probability for angles of 1,...,359 degrees that the drift/ram direction of the ship equals that specific angle.

.. figure::
    ./case1/area_overview.svg
    :alt: figure
    :align: center

    Test set-up

The traffic in the cell is defined with a uniform direction, so it is checked that the ram exposure is similar for each object.
Due to choices in the internal discretization, ram exposure is not precisely equal for each object,
but the deviations are small and this error will probably be negligible compared to uncertainties in the input parameters of SAMSON.
In the plot below the ram exposure is plotted as a function of the angle that is needed to reach the object.

For the drift direction, the drag induced speed vector is added to the current induced speed vector.
This implies that not every drift angle has equal probability. In this test, the drift angles are also computed manually and compared to the results of SAMSON.
The test passes if the difference between both methods is small enough. In the plot below the drift angle probabilities are compared for both methods.
In the plot, it can be observed that there is a difference between SAMSON and "manual" on the edge case.
This difference can be resolved by increasing the number of bins in the angle discretization.
The present difference is deemed acceptable.

Input
^^^^^

.. csv-table:: weatherstations.csv
   :file: ./case1/Area/weatherstations.csv
   :widths: auto
   :header-rows: 1

.. csv-table:: current.csv
   :file: ./case1/Area/current.csv
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

.. csv-table:: shipcelldata.csv
   :file: ./case1/ModelData/shipcelldata.csv
   :widths: auto
   :header-rows: 1

.. csv-table:: shipcells.csv
   :file: ./case1/Traffic/shipcells.csv
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
    ramexposure_uniform_cell.svg
    :alt: Comparison:Exposures:Link:Object:Elevation
    :align: center

    Drift/ram exposure versus the orientation of the object

.. _fig:Comparison:Exposures:Link:Object:Elevation:
.. figure::
    drift_direction_proportions.svg
    :alt: Comparison:Exposures:Link:Object:Elevation
    :align: center

    Drift/ram exposure versus the orientation of the object

.. literalinclude:: .check_output.txt