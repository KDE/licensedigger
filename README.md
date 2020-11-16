# License-Digger

This little tool helps to convert a project of source code files with traditional licenses headers to use statements that use the modern SPDX expressions.
The tool allows to perform one time conversions, as well as to rerun it on a codebase later on to catch newly added files.

## Out of Scope

The tools goal is only the _conversion_ of license headers. This means, explicitly the following features are not in scope (even though some are supported by accident):

- Linting the correctness of license statements your project. Use the `reuse lint` tool for this task.
- Generate an overview of used licenses in your project. `reuse spdx` might be your answer.
- Evaluating the outbound license of your compiled binaries. This is a difficult task with separate tooling.

## Usage

Simply run `licensedigger <your-project-root-directory>`. You will be presented the detected licenses and be asked if you want to proceed with the conversion.
For more options, see `licensedigger -h`.

### Trouble Shooting

#### Not all licenses are converted!

Due to its design, license digger is not guessing any licence but detects it only if it matches exactly (except white-spaces and asterisks) with a license statement from the internal license database.
You can easily add a new license to the database by the following steps:

1. Create a folder (in case it does not exist yet) in folder `licenses_templates/` that is named exactly as the target SPDX expression for the license header that you want to add. All white-spaces have to be replaced by underbars.
2. In that folder create a text file that contains the license header that you want to add. It must not contain any comments or indentations.
3. Edit `licenses.qrc` and add the newly created file.
4. If you added a completely new license, add the canonical license text to `licensetexts/` and also add this file in `licenses.qrc`.
5. Please use any source code that contains your new license header and add it to the unit tests (similar steps: add it to the correct `autotests/testsdata/` subfolder, edit `testdata.qrc`, modify the `headerdetection` unit test in case you added a new license.
6. Execute all tests in the build folder `ctest -R`.

#### Copyright statements are converted incorrectly!

Most copyright statements should be detected and converted correctly. If you have problematic cases, please fill an issue and add the original copyright statement.
All kinds of supported copyright statements are tested in the `copyrightconvert` unit test, in case you want to work on it yourself.

#### How can I cleanup asterisks and other cruft from the license statements?

Sorry, nothing is implemented yet, but patches are welcome :)

## Base Design Decisions

The ultimate goal of license digger is to ensure zero false license conversions. For this goal, other reasonable architecture goals are negatively affected.
For example:

- License statement files are added to the project by stating the containing license twice (naming in resource files and location of the license file) to minimize the likelihood of copy-paste errors.
- The scope of regular expressions for detecting license variants is very small per license (mostly comment markers and white-space). This means, for every tiny license variation a separate license statement file has to be added.
