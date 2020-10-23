#!/bin/sh

Xorg -noreset +extension GLX +extension RANDR +extension RENDER -config /etc/xorg.conf $DISPLAY &

exec "$@"