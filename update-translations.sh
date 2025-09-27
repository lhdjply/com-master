#!/bin/bash
export QT_SELECT=qt6
lupdate -recursive ./src/ ./myresource/ -ts ./myresource/src/translations/*.ts