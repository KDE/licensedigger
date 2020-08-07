# License-Digger

This little tool helps to convert a project of source code files with traditional licenses headers to use statements that use the modern SPDX expressions.
The tool allows to perform one time conversions, as well as to rerun it on a codebase later on to catch newly added files.

## Out of Scope

The tools goal is only the _conversion_ of license headers. This means, explicitly the following features are not in scope (even though some are supported by accident):

- Linting the correctness of license statements your project. Use the `reuse lint` tool for this task.
- Generate an overview of used licenses in your project. `reuse spdx` might be your answer.
- Evaluating the outbound license of your compiled binaries. This is a difficult task with separate tooling.


## Base Design Decisions

The ultimate goal of license digger is to ensure zero false license conversions. For this goal, other reasonable architecture goals are negatively affected.
For example:

- License statement files are added to the project by stating the containing license twice (naming in resource files and location of the license file) to minimize the likelhood of copy-paste errors.
- The scope of regular expressions for detecting license variants is very small per license (mostly comment markers and whitespace). This means, for every tiny license variation a separete license statement file has to be added.
