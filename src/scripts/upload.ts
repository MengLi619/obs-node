import * as util from 'util';
import * as os from 'os';
import * as gh from 'ghreleases';
import * as tar from 'tar-fs';
import * as fs from 'fs';
import * as path from 'path';
import * as stream from 'stream';

const ghGetByTag = util.promisify(gh.getByTag);
const ghCreate = util.promisify(gh.create);
const ghUploadAssets = util.promisify(gh.uploadAssets);
const pipeline = util.promisify(stream.pipeline);

const ghUser = process.env.GITHUB_USER;
const ghToken = process.env.GITHUB_TOKEN;
if (!ghUser || !ghToken) {
    throw new Error(`GITHUB_USER or GITHUB_TOKEN env is not provided.`);
}

const packageJson = JSON.parse(fs.readFileSync(path.resolve(__dirname, '../../package.json'), 'utf8'));
const repository = packageJson.repository || '';
let [owner, repoName] = repository.split('/').slice(3);
if (!owner || !repoName) {
    throw new Error(`Please provide a valid repository url in package.json`);
}
repoName = repoName.substring(0, repoName.lastIndexOf('.git'));

const version = packageJson.version;
const auth = {
    user: ghUser,
    token: ghToken
};

const platform = os.platform();
const assetsFileName = `prebuild-${platform}-${version}.tar.gz`;

(async () => {
    // create asset
    const prebuildFolder = path.resolve(__dirname, '../../prebuild');
    await pipeline(tar.pack(prebuildFolder), fs.createWriteStream(assetsFileName));

    // create release
    try {
        await ghGetByTag(auth, owner, repoName, version);
    } catch (e) {
        await ghCreate(auth, owner, repoName, {
            tag_name: version,
            name: `${repoName}-${version}`,
            body: `${repoName}-${version} release`,
        });
    }

    // upload asset file
    await ghUploadAssets(auth, owner, repoName, `tags/${version}`, [assetsFileName]);

    // delete tar file
    fs.unlinkSync(assetsFileName);
})();