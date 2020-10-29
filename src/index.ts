import * as os from 'os';
import * as path from 'path';

let obs: obs.ObsNode;
let cwd = process.cwd();
const isWindows = os.platform() === "win32";
try {
    if (isWindows) {
        // for windows, we need set working directory to obs binary path to load obs dependencies correctly.
        process.chdir(path.resolve(__dirname, '../prebuild/obs-studio/bin/64bit'));
    }
    obs = require('../prebuild/obs-node.node');
} finally {
    if (isWindows) {
        process.chdir(cwd);
    }
}

// set obs studio path before calling any function.
const obsPath = path.resolve(__dirname, '../prebuild/obs-studio');
obs.setObsPath(obsPath);

declare namespace obs {

    export type RateControl = 'CBR';

    export type SourceType = 'Image' | 'MediaSource';

    export type TransitionType = 'cut_transition' | 'fade_transition' | 'swipe_transition' | 'slide_transition';

    export interface Scene {
        id: string;
        source: Source[];
    }

    export interface Source {
        id: string;
        type: SourceType;
        url: string;
    }

    export interface VideoSettings {
        baseWidth: number;
        baseHeight: number;
        outputWidth: number;
        outputHeight: number;
        fpsNum: number;
        fpsDen: number;
    }

    export interface AudioSettings {
        sampleRate: number;
    }

    export interface VideoDecoderSettings {
        hardwareEnable: boolean;
    }

    export interface VideoEncoderSettings {
        hardwareEnable: boolean;
        width: number;
        height: number;
        bitrateKbps: number;
        keyintSec: number;
        rateControl: RateControl;
        preset: string;
        profile: string;
        tune: string;
        x264opts?: string;
    }

    export interface AudioEncoderSettings {
        bitrateKbps: number;
    }

    export interface OutputSettings {
        server: string;
        key: string;
    }

    export interface Settings {
        video: VideoSettings;
        audio: AudioSettings;
        videoDecoder?: VideoDecoderSettings;
        videoEncoder?: VideoEncoderSettings;
        audioEncoder?: AudioEncoderSettings;
        output?: OutputSettings;
    }

    export interface Bounds {
        x: number;
        y: number;
        width: number;
        height: number;
    }

    export interface ObsNode {
        setObsPath(obsPath: string): void
        startup(settings: Settings): void;
        shutdown(): void;
        addScene(sceneId: string): string;
        addSource(sceneId: string, sourceId: string, sourceType: SourceType, sourceUrl: string): void;
        updateSource(sceneId: string, sourceId: string, sourceUrl: string): void;
        muteSource(sceneId: string, sourceId: string, mute: boolean): void;
        restartSource(sceneId: string, sourceId: string);
        switchToScene(sceneId: string, transitionType: TransitionType, transitionMs: number): void;
        getScenes(): Scene[];
        createDisplay(name: string, parentWindow: Buffer, scaleFactor: number, sourceId: string);
        destroyDisplay(name: string);
        moveDisplay(name: string, x: number, y: number, width: number, height: number);
    }
}

export = obs;