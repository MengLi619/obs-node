import * as os from 'os';
import * as github from 'github-from-package';
import * as path from 'path';
import * as fs from 'fs';
import axios from 'axios';
import * as tar from 'tar-fs';
import * as stream from 'stream';
import * as util from 'util';

const packageJson = JSON.parse(fs.readFileSync(path.resolve(__dirname, '../../package.json'), 'utf8'));
const pipeline = util.promisify(stream.pipeline);

function getDownloadUrl() {
    const version = packageJson.version;
    const platform = os.platform();
    const url = github(packageJson);
    return `${url}//releases/download/${version}/prebuild-${platform}-${version}.tar.gz`;
}

async function download(url: string, fileName: string) {
    const writer = fs.createWriteStream(fileName)
    const response = await axios({
        url,
        method: 'GET',
        responseType: 'stream',
        headers: {
            'User-Agent': 'npm download',
            Accept: 'application/octet-stream',
        },
    });
    response.data.pipe(writer);
    return new Promise((resolve, reject) => {
        writer.on('finish', resolve);
        writer.on('error', reject);
    });
}

const url = getDownloadUrl();
const fileName = url.substring(url.lastIndexOf('/') + 1);
const downloadFolder = path.resolve(__dirname, '../../');
const tarFile = path.join(downloadFolder, fileName);
const untarFolder = path.join(downloadFolder, 'prebuild');

// if (!(packageJson as any)._from) {
//     console.log(`Local install, skip download prebuild.`);
//     process.exit(0);
// }

if (fs.existsSync(untarFolder)) {
    console.log(`Prebuild folder is already existed, skip download.`);
    process.exit(0);
}

(async () => {
    try {
        // download prebuild tar file
        console.log(`Downloading ${url} to ${tarFile}`);
        await download(url, tarFile);

        // uncompress the tar file
        console.log(`Uncompressing ${tarFile} to ${untarFolder}`);
        await pipeline(fs.createReadStream(tarFile), tar.extract(untarFolder));
    } finally {
        if (fs.existsSync(tarFile)) {
            fs.unlinkSync(tarFile);
        }
    }
})();