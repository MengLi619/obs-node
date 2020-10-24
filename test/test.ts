import * as obs from '../src';
import { SourceType } from "../src";
import * as readline from 'readline';

interface Source {
    sceneId: string;
    sourceId: string;
    sourceType: SourceType;
    sourceUrl: string;
}

const settings: obs.Settings = {
    server: 'rtmp://host.docker.internal/live',
    key: 'output',
    videoHWDecode: false,
    videoHWEncode: false,
    videoGpuConversion: true,
    videoBitrateKbps: 5000,
    videoKeyintSec: 1,
    videoRateControl: 'CBR',
    videoWidth: 1280,
    videoHeight: 720,
    videoFpsNum: 25000,
    videoFpsDen: 1000,
    audioSampleRate: 44100,
    audioBitrateKbps: 64,
    preset: 'ultrafast',
    profile: 'main',
    tune: 'zerolatency',
    x264opts: '',
};

const sources: Source[] = [
    {
        sceneId: 'scene1',
        sourceId: 'source1',
        sourceType: 'MediaSource',
        sourceUrl: 'rtmp://host.docker.internal/live/source1',
    },
    {
        sceneId: 'scene2',
        sourceId: 'source2',
        sourceType: 'MediaSource',
        sourceUrl: 'rtmp://host.docker.internal/live/source2',
    }
];

obs.startup(settings);
sources.forEach(s => {
    obs.addScene(s.sceneId);
    obs.addSource(s.sceneId, s.sourceId, s.sourceType, s.sourceUrl);
});

console.log(`Obs scenes: ${JSON.stringify(obs.getScenes())}`);

const readLine = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
    terminal: true,
});

const question = (callback: (sceneId: string) => void) => {
    sources.forEach(s => readLine.write(`${s.sceneId}\n`));
    readLine.question('Which scene to switch?\n', async (sceneId: string) => {
        try {
            if (sceneId) {
                await callback(sceneId);
            }
        } finally {
            question(callback);
        }
    });
};

question(sceneId => {
    obs.switchToScene(sceneId, 'cut_transition', 1000);
});