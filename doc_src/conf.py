# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'Dive Computer'
copyright = 'Copyright 2024, Alexey Vlasov, Maxim Parinov. Licensed under the Apache License, Version 2.0'
author = 'Alexey Vlasov, Maxim Parinov'
release = 'v0.1'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = []

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

language = 'en'

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = "sphinx_rtd_theme"
html_static_path = ['_static']

html_favicon = "_static/favicon.png"
html_logo = "_static/dive_computer_logo.svg"

html_theme_options = {
  "style_nav_header_background" : "#40a9ed"
}
