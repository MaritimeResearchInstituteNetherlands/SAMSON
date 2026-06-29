Link Discretization
-------------------

General
^^^^^^^

:Objective:
  Verify that ship link object exposure does not change when the link is splitted into multiple parts
:Criteria:
  The difference between the normal link exposure and the sum of the splitted link exposures should not exceed 3%

Three different cases are considered: unidirectional wind, unidirectional current and a more standard scenario with multiple wind directions and strengths.

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

.. _fig_Comparison_Exposures_Link_Split:

.. figure:: figure1.svg
   :alt: Comparison:Exposures:Link:Split
   :align: center

   Drift/ram exposure for the different environment cases for the normal link and the splitted link

.. literalinclude:: .check_output.txt