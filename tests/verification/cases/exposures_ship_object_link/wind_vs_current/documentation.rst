Wind versus Current
-------------------

General
^^^^^^^

:Objective:
  Verify that ship object exposure behaves the same for wind and current
:Criteria:
  Exposure between the wind and current cases should be the same.

Two cases are considered with one shiptype modelled on one link and one cell:

1. Wind bft 5 to the east, no current or tide

2. Wind bft 0, current equal to the drag velocity corresponding to bft 5, no tide

.. figure:: ./current/area_overview.svg
   :alt: figure
   :align: center

   Test set-up

Input
^^^^^

.. csv-table:: weatherstations.csv
   :file: ./current/Area/weatherstations.csv
   :widths: auto
   :header-rows: 1

.. csv-table:: windstrength.csv
   :file: ./current/Area/windstrength.csv
   :widths: auto
   :header-rows: 1

.. csv-table:: winddirection.csv
   :file: ./current/Area/winddirection.csv
   :widths: auto
   :header-rows: 1

.. csv-table:: shipcategories.csv
   :file: ./current/Traffic/shipcategories.csv
   :widths: auto
   :header-rows: 1

.. csv-table:: shiplinkdata.csv
   :file: ./current/ModelData/shiplinkdata.csv
   :widths: auto
   :header-rows: 1

.. csv-table:: shiplinks.csv
   :file: ./current/Traffic/shiplinks.csv
   :widths: auto
   :header-rows: 1

.. csv-table:: objects.csv
   :file: ./current/Area/objects.csv
   :widths: auto
   :header-rows: 1

Result
^^^^^^

.. _fig_Comparison_Exposures_Link_Object_Wind_Current:

.. figure:: figure1.svg
   :alt: Comparison:Exposures:Link:Object:Wind:Current
   :align: center

   Drift exposure for both cases

.. literalinclude:: .check_output.txt