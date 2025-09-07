# Contributing to ifupdown-ng

This document attempts to outline the key things for contributors to be
aware of when submitting contributions to ifupdown-ng.

## Development process

ifupdown-ng is one of many components in the "system software" space, and
is critical load-bearing infrastructure for several distributions.

This means that the development process is largely informed by demand for
new releases from these distributions.  For example, the majority of
ifupdown-ng releases have been cut because there was a release-critical
bug in Alpine or another distribution which uses ifupdown-ng, requiring
a release to solve the problem.

Prospective contributors should therefore be aware that, as development
sprints happen on an as-needed basis, reviews of patches may be delayed
until the next development sprint, which could be months to years in the
future.

System software has a very hibernatory development process, largely because
introducing changes to system software carries significant risk.  As the
old saying goes, "If it isn't broken, don't fix it."

Similarly, if a bug is tolerable, meaning that it does not cause
release-critical regressions in the downstream distributions which consume
ifupdown-ng, then it will likely be dealt with in the next development
sprint.  This is just a matter of practicality, the maintainers of
ifupdown-ng are heavily involved in many other projects.

## Copyright

All contributions to ifupdown-ng are presumed to be the direct intellectual
property of the contributor, or that the contributor has the legal authority
to otherwise submit the contribution under the license terms laid out in
COPYING.

In simple terms, this means that:

 * You (and/or any other relevant parties) retain the copyrights to your
   contribution.

 * You are required to contribute these contributions under the ISC license,
   as depicted in COPYING.

 * You are required to obtain consent for any third party code which you
   are contributing to ifupdown-ng to be distributed under the ISC license.

   Most permissive non-copyleft licenses already include a blanket clause
   granting permission to redistribute and sublicense the content they are
   covering.

 * The ifupdown-ng software distribution copyright is held by an amalgamation
   of the contributors to ifupdown-ng, including *you* when your contributions
   are accepted and merged.

## Suggested strategy

In general, we prefer smaller pull requests to larger ones.  Even if a feature
requires a lot of code to support it, we have found that breaking the
implementation down into smaller PRs allows for more focused review cycles,
as well as more generalized design approaches.

Monolithic PRs, on the other hand, tend to result in coding/design decisions
which favor supporting individual features, which is unfortunate because 
more generalized design work can result in much less work having to be done to
support similar features in the future.

As maintainers, we want to be taken on a journey while reviewing your pull
request.  We want to understand the thought process that went into developing
a feature.  We want to understand the mistakes that were encountered along the
way.  Being able to trace the lineage of a feature's development is highly
valuable to the next person who has to work with the code, and more importantly,
triage bugs in the code.

The takeaway here is that we want features to be spoon-fed to us across multiple
PRs, ideally one PR for each underlying concern addressed during the development
of the feature.  Likewise, we *do not want* pull requests which are large and
monolithic, as these require significantly greater effort to review and consider.

If you are unsure whether or not a feature should be implemented, run it by us
first.

Do not squash your commit history, the most important code artifacts can be the
commits where mistakes are fixed.  This is because they inform us to think about
those mistakes -- why did they happen, what was the impact -- both when reviewing
your code in the present, but also when we are triaging bug reports against it
in the future.  Rebasing to remove "fixup commits" is fine, as long as the overall
path of the feature's development is preserved.

Follow the coding style as used in the rest of the project.  If you see code which
does not follow the coding style exactly, do not fix it, as it will just generate
additional noise to sift through when triaging bugs in the future.

## General design notes

ifupdown-ng is largely focused on configuration aspects where it will be the only
tool managing the configured interfaces.  In general, this means that spawning a
long-lived daemon as part of an executor is probably the wrong thing.  This also
means that executors which only facilitate the creation of an interface which will
be managed by another program should be avoided, instead those programs should
create the interfaces they will otherwise be managing.

As outlined in the manual pages, devices managed by ifupdown-ng go through 7 phases,
which result in executors being called for each one.  These phases are distinct
and should do only the tasks relevant to each phase.  For example, it is not acceptable
for an interface to be created in any phase other than the 'create' phase.

Features should be thought of in terms of new semantic vocabularies.  For example,
declared directives for new features should be prefixed with the feature's name.  If
we consider a new executor feature `vrrp`, for example, the directives should be
named `vrrp-foo`, `vrrp-bar`, etc.  This helps to allow the configuration to be cleanly
modeled in other semantic configuration frameworks, such as NETCONF.
