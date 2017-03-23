# Change Log

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/).

## [Unreleased]

### Added

- AppVeyor continuous integration builds.

## 0.2.0

This update "breaks" compatibility with 0.1.0 by ensuring read_mem
yields numbers of the range 0 to 255 rather than -127 to 127. This was
the intended behavior.

## 0.1.0

Minimum viable product.
