# ⚠️ IMPORTANT: DO NOT EDIT FILES IN THIS DIRECTORY

This directory contains the default configuration files for **Cache Cleaner**. 
These files are automatically installed and **will be overwritten** every time you update the application.

### What is inside?
* `categories.json` — the list of built-in cache paths and cleaning commands.
* `package_managers.json` — the mapping of Linux distributions to their package managers.
* `app_style.css` — the default user interface theme.

### How to add your own custom caches?
If you want to add your own cache directories (or commands), **do not edit the files here**. 
Instead, create a file at:
`~/.config/cache-cleaner/user_directories.json`

*(See the main GitHub repository's documentation for the format of this file).*

### What if I accidentally broke a file?
If you edited a file here and broke it, you can simply reinstall the application (or download the original file from the GitHub repository) to replace the damaged one.