## SAMSON

----------------------
SAMSON is a quantitative maritime collision frequency model framework developed at MARIN.
Development is primarily conducted internally at MARIN, with this repository serving as a public-facing mirror and collaboration point.
While the codebase, tests, and documentation are available for external use, 
certain internal MARIN tooling and workflows are not included; 
external users are therefore expected to configure 
and run the software independently within their own environments.

Pre-built HTML documentation is available in `doc/build`. 
After cloning the repository, 
open `index.html` in your web browser to access the documentation.
## Project Status

⚠️ **Alpha release (internal use)**  
This software is under active development and not yet intended for production use.
For external users, we suggest to get in contact with MARIN (samsonsupport@marin.nl) 
so we can guide the user directly with troubleshooting until we have a release. 
The release of SAMSON 2.0 is scheduled for the end of 2026. For the release the following activities are still planned:
- Clean up and clarification of the code 
- Additional performance optimisation
- Update of the theory documentation
- Addition of tutorials and best practices
- Finalisation of feature requests
- Extension of the testing period to ensure a stable release


## Contributing
At this stage, SAMSON 2.0 is primarily developed internally at MARIN.

You are free to:
- Open issues to report bugs or request features
- Submit pull requests to propose changes

Please note:
- Active development currently takes place internally at MARIN
- Pull requests are reviewed on a best-effort basis
- Contributions may be adapted, partially used, or integrated internally (e.g. via cherry-picking)
- Submission of a pull request does not guarantee inclusion in the main codebase
- By submitting a contribution, you agree that your contributions will be licensed under the EUPL v1.2.

If you are planning a larger contribution, we recommend contacting us first via samsonsupport@marin.nl.

## Support

- For general questions and guided support: samsonsupport@marin.nl
- For bugs and feature requests: please use GitHub Issues

### Way of working (internal)
> ⚠️ The following workflow applies to internal MARIN development. External contributors are not expected to follow this process.

This project uses gitflow. <br>
There are two ever-living branches in the repository, namely `main` and `develop`:
- `develop` is the integration branch for new work. I.e. all `feature/`-branches branch from `develop` and merge back into `develop`.
- `main` is the branch for all releases. Each commit on `main` contains a fully working version of the codebase.

Changes onto `main` and `develop` are only allowed via pull requests.

All other branches on remote must satisfy the gitflow branching model. This means that all branches should be prefixed by one of:

- `feature/` for branches containing new features. `feature/`-branches branch from `develop` and merge into `develop`.
- `bugfix/` for branches containing fixes, no new functionality. `bugfix/`-branches branch from `develop` or `release/` and merge into `develop` or `release/`.
- `release/` for branches containing as new set of features to be released. `release/`-branches branch from `develop` and merge into `main`.
- `hotfix/` for branches containing fixes, no new functionality.
`hotfix/`-branches branch from `main` and merge into `main` <u>and</u> `develop`.

You can find a detailed explanation about using gitflow [on this webpage](https://www.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow).

To keep changes traceable, each pull request must include a reference to a Jira ticket. This can be done in one of three ways:

- Ensure that the source branch name also includes the issue key in the branch name. [Read this section below](#use-atlassian---integration) to find out how to do that.

- Include the issue key in the pull request title.

- Include a commit in the pull request that has the issue key in the commit message. Note, the commit cannot be a merge commit.

----------------------
### Tips & best practices

#### `feature/`-branches are yours!

Commits on feature branches do not have to contain working code. The branch is separate from develop; you are not inconveniencing anyone if you experiment and break things in a feature branch. Use them accordingly.

#### Use Atlassian - integration

In every jira ticket there is a heading named `development`. Within that you can click on a link named [create branch](https://git.marin.nl/plugins/servlet/create-branch/projects/SAMSON/repos/samson). Jira then redirects you to a page that already generates a correct name for your branch. All you have to do is select the branch type you wish to create. Read more about the integration [here](https://support.atlassian.com/jira-software-cloud/docs/view-development-information-for-an-issue/#Create-feature-branches).

#### Building on changes in review

If you want to build on changes in a feature branch (`feature/B`) from another feature branch (`feature/A`) that has not yet been merged into develop, do the following:
- Create `feature/B` locally, as a branch of `feature/A`.
- Build your functionality, commit, but don't push.
- Wait for the merge of `feature/A` --> `develop` to complete.
- Fetch latest version of `develop`.
- Do a rebase of `feature/B` onto `develop`.
- Push `feature/B` to remote.

[Check out this link](https://www.atlassian.com/nl/git/tutorials/rewriting-history/git-rebase) to learn about rebase.


#### Keep commits small and commit messages descriptive
The commit log describes in a logical narrative how a particular change came about. If you have been working on a feature for a long time and have changed many files, take the time to break down your changes into logical chunks and commit them.<br>

So don't put too many changes into one commit:
- Add /compute route to app and handle result and add tests

Rather, split the changes into multiple commits:
- Define /compute route in routes.py
- Define validation schema for /compute-request
- Add handle_compute() function to backend
- Define validation schema for handle_compute() result
- Add mock for /compute in tests/

Be descriptive in your commit messages. Write:
- Fix type for field 'speed' in validation scheme

instead of:
- Fixed bug

For more explanation on the importance of good commit messages, see [here](https://blog.mocoso.co.uk/posts/talks/telling-stories-through-your-commits/) or [here](https://cbea.ms/git-commit/).

#### Code-style
The Google C++ Style Guide (https://google.github.io/styleguide/cppguide.html) is used as reference with the following deviations:
- Use #pragma once instead of header guards. It is easier to use and does the work just fine
- Use tabs for indentation. Editor can sort out the actual length of the tabs.
- Use spaces when you want to format a table or something in comments

#### Sourcetree

Although VS Code and Pycharm have built-in tools to commit, branch and push with git, that's not visual enough for everyone.
Sourcetree is available in K1000; an accessible, graphical git client. In Sourcetree, the distinction between local and remote branches is clear, you can easily split large changes into small commits, and there are clear alerts when changes have occurred on the remote that may be useful to you.
 
----------------------
### Pitfalls

#### Foxtrot merges

Foxtrot merges are blocked by the repository, because they break the git history, practically making the diff-functionality in pull requests unusable. <br>
Read more about what foxtrot merges are and how to prevent them [here](https://mods.marin.nl/pages/viewpage.action?pageId=161355264) and [here](https://blog.developer.atlassian.com/stop-foxtrots-now/).

# Building the Code on Windows

1- Install CMAKE: 

- Download zipped version  4.3.1 or later version CMAKE. [Link](https://cmake.org/download/)

- Add unzipped folder to PATH environment variable.

2- Install vcpkage dependency management tool by following the official instructions. [Link](https://github.com/microsoft/vcpkg?tab=readme-ov-file#quick-start-windows)
- Set the installation path of vcpkg to VCPKG_ROOT environment variable.

3- Project needs an install directory for created libraries, header files and some other files:
- Create a folder anyplace suitable for you
- Set SAMSON_INSTALL_PATH environment variable to the folder path
- Add SAMSON_INSTALL_PATH to PATH environment variable. You will need this while running test cases to find shared libraries. 

4- Open CMAKE gui and set followings:
- 'Where is the source code' to the project folder (the folder where you find this readme file)
- 'Preset' to 'default'.
- 'Where to build binaries' to '[project folder]/build'. This will create the build folder.

![alt text](doc/readme_01.png)

5- Click Configure and then Generate to create a visual studio project:
![alt text](doc/readme_02.png)

6- Check the build folder you must see 'SamsonCore.sln'. This is the main project where you should open to build the project.

7- If you have not installed Visual Studio, this is the time. Create an IT ticket for this. Currently, Visual Studio 2026 is used.

8- When you open the project you will see "CMakePredefinedTargets" you need to build "ALL_BUILD" and then build "INSTALL". Now your project is ready for unit test execution.

9- Adding "_tests" to the module project names is the current convention for unit test projects. Unit test projects are created alongside the modules."SamsonCore" is the main module and "SamsonCore_Test" is the unit test project for it. You will see other unit tests alongside module projects. "trafficdb_tests" is a good start point to develop familiarity.

![alt text](doc/readme_03.png)

# Running Samson CLI 
Samson CLI is a command line tool designed to showcase the capabilities of Samson. A basic example study is provided in SAMSON/CLI/Sample. To run Samson CLI for this example, locate SamsonCLI.exe, navigate to SAMSON/CLI/Sample and execute:
```bash
 .\SamsonCLI.exe --config-yaml "config.yaml" --actions "actions.txt"
```

## License

This project is licensed under the European Union Public Licence (EUPL) v1.2.

You may not use this work except in compliance with the Licence.
You may obtain a copy of the Licence at:

https://eupl.eu/1.2/en/

See the LICENSE file for the full text.
