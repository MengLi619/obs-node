import * as os from 'os';
import * as path from 'path';

process.chdir(path.resolve(__dirname, '../prebuild/bin/64bit'));

let obs: obs.ObsNode;
switch(os.platform()) {
    case 'win32':
    case 'linux':
        obs = require('../prebuild/bin/64bit/obs-node.node');
        break;
    case 'darwin':
        obs = require('../prebuild/bin/obs-node.node');
        break;
    default:
        throw new Error(`Unsupported platform: ${os.platform()}`);
}

const obsPath = path.resolve(__dirname, '../prebuild');
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

    export interface Settings {
        server: string;
        key: string;
        videoHWDecode: boolean;
        videoHWEncode: boolean;
        videoGpuConversion: boolean;
        videoBitrateKbps: number;
        videoKeyintSec: number;
        videoRateControl: RateControl;
        videoWidth: number;
        videoHeight: number;
        videoFpsNum: number;
        videoFpsDen: number;
        audioSampleRate: number;
        audioBitrateKbps: number;
        preset: string;
        profile: string;
        tune: string;
        x264opts: string;
    }

    export interface ObsNode {
        setObsPath(obsPath: string): void
        startup(settings: Settings): void;
        shutdown(): void;
        addScene(sceneId: string): string;
        addSource(sceneId: string, sourceId: string, sourceType: SourceType, sourceUrl: string): void;
        restartSource(sceneId: string, sourceId: string);
        switchToScene(sceneId: string, transitionType: TransitionType, transitionMs: number): void;
        getScenes(): Scene[];
    }
}

export = obs;