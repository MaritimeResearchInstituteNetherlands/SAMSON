Ram Verification for Gridlines
------------------------------

General
^^^^^^^

:Objective:
  Verify that the calculated ram exposure is consistent with the theoretical model
:Criteria:
  - The ram exposure should be monotonically decreasing as a function of the y_distance between link and object
  - The ram exposure should be similar between cells and links that are approximately equal to each other
  - The ram exposure should be symmetric as a function of the x_distance between link and object
  - The SAMSON exposure should not deviate too much from approximations calculated by hand

A grid of line objects is created right in front of the link. SAMSON is used to calculate ram exposure.
Then, the results are checked for consistency based on the above criteria.
In the below plots, it can be observed that up to 10km, the link and cell ram exposure model agree rather well with each other.
When the distance between traffic and object exceeds 10km, the cell ram exposure starts to decrease faster.
This behaviour is caused by the fact that subcells too far away from the object are not taken into account.
The difference is deemed acceptable, because it is not observed in the comparison between ram collisions.
In the heatmaps comparing the link with the cell, we also see this transition at a distance of apprximately 10km.

Also, we see that as a result of discretisation, there is a difference between ram exposure for objects to the left and the right of the object.

For this test, the ram exposure and ram collisions are also approximated in a python script.
The test fails if the results of these calculations differ too much from the values calculated by SAMSON.

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
    link_vs_cell_ramexp.svg
    :alt: Comparison:Exposures:Link:Object:Elevation
    :align: center

    Ram exposure as a function of the distance between the platform to the link

.. _fig:Comparison:Exposures:Link:Object:Elevation:
.. figure::
    link_vs_cell_ramcol.svg
    :alt: Comparison:Exposures:Link:Object:Elevation
    :align: center

    Ram collisions as a function of the distance between the platform to the link

.. _fig:Comparison:Exposures:Link:Object:Elevation:
.. figure::
    test.svg
    :alt: Comparison:Exposures:Link:Object:Elevation
    :align: center

    The ratio between ram exposure for objects to the left and right of the link/cell

.. _fig:Comparison:Exposures:Link:Object:Elevation:
.. figure::
    link_heatmap.svg
    :alt: Comparison:Exposures:Link:Object:Elevation
    :align: center

    Heatmap of ram exposure as a function of the location of the object.

.. _fig:Comparison:Exposures:Link:Object:Elevation:
.. figure::
    link_slice.svg
    :alt: Comparison:Exposures:Link:Object:Elevation
    :align: center

    Ram exposure as the function of the x distance between link and object for different values for the y distance
.. _fig:Comparison:Exposures:Link:Object:Elevation:
.. figure::
    cell_slice.svg
    :alt: Comparison:Exposures:Link:Object:Elevation
    :align: center

    Ram exposure as the function of the x distance between cell and object for different values for the y distance

.. _fig:Comparison:Exposures:Link:Object:Elevation:
.. figure::
    exposure_ratio.svg
    :alt: Comparison:Exposures:Link:Object:Elevation
    :align: center

    The ratio between values calculated by SAMSON and values approximated by hand for ram exposure and ram collisions

.. _fig:Comparison:Exposures:Link:Object:Elevation:
.. figure::
    driftExposure_ratio_heatmap.svg
    :alt: Comparison:Exposures:Link:Object:Elevation
    :align: center

    Heatmap of the ratio obtained by dividing link exposure by cell exposure

.. _fig:Comparison:Exposures:Link:Object:Elevation:
.. figure::
    ramExposure_ratio_heatmap.svg
    :alt: Comparison:Exposures:Link:Object:Elevation
    :align: center

    Heatmap of the ratio obtained by dividing link exposure by cell exposure

.. literalinclude:: .check_output.txt