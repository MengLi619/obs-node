import * as path from 'path';

const obs = require('../prebuild/obs-node.node');

export type RateControl = 'CBR';

export type SourceType = 'Image' | 'MediaSource';

export type TransitionType = 'cut_transition';

export interface Settings {
    showName: string;
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

export function startup(settings: Settings): boolean {
    const obsPath = path.resolve(__dirname, '../prebuild/obs-studio');
    obs.setObsPath(obsPath);
    return obs.startup(settings);
}

export function shutdown() {
    obs.shutdown();
}

export function addScene(sceneId: string) {
    obs.addScene(sceneId);
}

export function addSource(sceneId: string, sourceId: string, sourceType: SourceType, sourceUrl: string) {
    obs.addSource(sceneId, sourceId, sourceType, sourceUrl);
}

export function switchToScene(sceneId: string, transitionType: TransitionType, transitionMs: number) {
    obs.switchToScene(sceneId, transitionType, transitionMs);
}