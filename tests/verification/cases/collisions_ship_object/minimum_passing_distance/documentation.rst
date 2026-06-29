Minimum passing distance
------------------------------

General
^^^^^^^

:Objective:
  Verify that the minimum passing distance measure is implemented correctly
:Criteria:
  The drift collision frequency should be constant for objects within 500m of the link

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

.. _fig_Exposures_Link_Object_Quantified:

.. figure:: figure1.svg
   :alt: Exposures:Link:Object:Quantified
   :align: center

   Number of drift collisions versus the distance of the platform to the link

.. literalinclude:: .check_output.txt