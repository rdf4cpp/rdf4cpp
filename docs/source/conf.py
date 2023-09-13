# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html


import subprocess, os, sys
sys.path.append("/home/mcb/other_c/breathe/")
sys.path.append("/home/mcb/other_c/exhale/")
subprocess.call('cd ../../; doxygen', shell=True)
#subprocess.call('cd ../../; python3 /home/mcb/other_c/breathe/breathe-apidoc.py -T -g file -o ./docs/source/ ./doc/xml/', shell=True)

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'rdf4cpp'
copyright = '2023, DICE Group'
author = 'DICE Group'
release = '0.0.14'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ["breathe", "exhale"]

templates_path = ['_templates']
exclude_patterns = []



# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'alabaster'
html_static_path = ['_static']

breathe_projects = {
    "rdf4cpp": "../../doc/xml/",
}
breathe_default_project = "rdf4cpp"
breathe_default_members = 'members'

# Setup the exhale extension
exhale_args = {
    # These arguments are required
    "containmentFolder":     "./api",
    "rootFileName":          "library_root.rst",
    "doxygenStripFromPath":  "..",
    # Heavily encouraged optional argument (see docs)
    "rootFileTitle":         "Library API",
    # Suggested optional arguments
    "createTreeView":        True,
    # TIP: if using the sphinx-bootstrap-theme, you need
    # "treeViewIsBootstrap": True,
    "exhaleExecutesDoxygen": False,
    "exhaleDoxygenStdin":    "INPUT = ../include"
}

# Tell sphinx what the primary language being documented is.
primary_domain = 'cpp'

# Tell sphinx what the pygments highlight language should be.
highlight_language = 'cpp'