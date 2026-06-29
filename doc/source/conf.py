import os

project = "SAMSON"
version = "develop"
release = "none"
copyright = "MARIN"

ROOT_URL = 'https://srcdoc.marin.nl'

versions_url = ROOT_URL + "/" + project + '/versions.json'


def generate_index_rst(directory, title):
    """Generate an index.rst file listing subdirectories as a toctree."""
    subdirs = []

    for entry in sorted(os.listdir(directory)):
        full_path = os.path.join(directory, entry)
        if os.path.isdir(full_path) and not entry.startswith("."):
            subdirs.append(entry)

    index_path = os.path.join(directory, "index.rst")
    with open(index_path, "w") as f:
        f.write(f"{title}\n{'-' * len(title)}\n\n")
        f.write("  .. toctree::\n")
        f.write("      :glob:\n")
        f.write("      :maxdepth: 1\n\n")
        for subdir in subdirs:
            f.write(f"      {subdir}/*\n")


# ---------------------------------------------------------------------
# Build index.rst files for test results
# ---------------------------------------------------------------------

ROOT_PATH_TESTS = os.path.join(os.getcwd(), "tests", "test_results", "cases")

generate_index_rst(
    os.path.join(ROOT_PATH_TESTS, "exposures_ship_ship_link_link"),
    "Link-Link",
)
generate_index_rst(
    os.path.join(ROOT_PATH_TESTS, "exposures_ship_ship_cell"),
    "Cell",
)
generate_index_rst(
    os.path.join(ROOT_PATH_TESTS, "exposures_ship_ship_link_cell"),
    "Link-Cell",
)
generate_index_rst(
    os.path.join(ROOT_PATH_TESTS, "exposures_ship_object_link"),
    "Link-Object",
)
generate_index_rst(
    os.path.join(ROOT_PATH_TESTS, "exposures_ship_object_cell"),
    "Cell-Object",
)
generate_index_rst(
    os.path.join(ROOT_PATH_TESTS, "events_ship_ship"),
    "Causation Factors",
)
generate_index_rst(
    os.path.join(ROOT_PATH_TESTS, "events_ship_object"),
    "Causation Factors",
)
# generate_index_rst(
    # os.path.join(ROOT_PATH_TESTS, "collisions_ship_ship"),
    # "Measures",
# )
generate_index_rst(
    os.path.join(ROOT_PATH_TESTS, "collisions_ship_object"),
    "Measures",
)

# ---------------------------------------------------------------------
# Sphinx Theme Configuration
# ---------------------------------------------------------------------

html_theme = "pydata_sphinx_theme"

html_theme_options = {
    "show_prev_next": False,
    "article_footer_items": [],
    "footer_start": [],
    "footer_end": [],
    "navbar_start": ["navbar-logo", "version-switcher"],
    "navigation_with_keys": False,
    "switcher": {
        "json_url": versions_url,
        "version_match": version,
    },
}

html_css_files = ["custom.css"]

html_sidebars = {
    "release_notes": []
}

extensions = [
    "sphinxcontrib.bibtex",
    "sphinx_copybutton",
    "sphinx_design",
]

source_suffix = [".rst"]
root_doc = "index"

autoclass_content = "both"
language = "en"

exclude_patterns = ["_build", "_env"]
modindex_common_prefix = ["samson."]
templates_path = ["_templates"]

add_function_parentheses = True
add_module_names = True

pygments_style = "sphinx"

html_title = "samson"
html_short_title = "samson"
html_logo = "./logo/logo.png"

html_use_index = True
html_copy_source = False
html_show_sourcelink = False
html_show_sphinx = False
html_show_copyright = False

htmlhelp_basename = "samson_doc"

bibtex_bibfiles = ["references.bib"]

numfig = True
math_numfig = True

set_html_assets_policy = "always"