Waterproof Computer Body
========================================
Then click the :guilabel:`Main Test` button to create the poject and open the.

#. Navigate back to the and click the :guilabel:`⚙ Admin` button,to open the Settings page.


.. code-block:: yaml
   :caption: .readthedocs.yaml
   :emphasize-lines: 6

   version: 2

   build:
     os: "ubuntu-22.04"
     tools:
       python: "3.8"

   python:
     install:
       - requirements: docs/requirements.txt

   sphinx:
     configuration: docs/source/conf.py


.. tip::

    Take into account that if the title changes, your ``section`` argument will break.
    To avoid that, you can manually define Sphinx references above the sections you don't want to break.
    For example,

    .. tabs::

       .. tab:: reStructuredText

          .. code-block:: rst
             :emphasize-lines: 3

             .. in your .rst document file

             .. _unbreakable-section-reference:

             Creating an automation rule
             ---------------------------

             This is the text of the section.

       .. tab:: MyST (Markdown)

          .. code-block:: md
             :emphasize-lines: 3

             .. in your .md document file

             (unbreakable-section-reference)=
             ## Creating an automation rule

             This is the text of the section.
