Sublink test
----------------

General
^^^^^^^

:Objective:
  Verify that larger sigma's work as expected
:Criteria:
  The exposure should be the same when a wide sublink is spread over multiple small sublinks

.. figure:: ./case1/area_overview.svg
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

.. _fig_Comparison_Exposures_Link_Object_Rotation:

.. figure:: figure1.svg
   :alt: Comparison:Exposures:Link:Object:Rotation
   :align: center

   Drift/ram exposure versus the orientation of the object

.. literalinclude:: .check_output.txt