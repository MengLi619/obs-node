import * as path from 'path';

const obs: obs.ObsNode = require('../prebuild/obs-node.node');
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