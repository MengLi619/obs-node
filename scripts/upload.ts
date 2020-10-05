import { promisify } from "util";
import * as os from 'os';
import * as gh from 'ghreleases';
import * as tar from 'tar';
import * as packageJson from '../package.json';
import * as path from "path";
import * as fs from "fs";

const ghUser = process.env.GITHUB_USER;
const ghToken = process.env.GITHUB_TOKEN;
if (!ghUser || !ghToken) {
    throw new Error(`GITHUB_USER or GITHUB_TOKEN env is not provided.`);
}

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

const projectName = packageJson.name;
const platform = os.platform();
const assetsFileName = `prebuild-${projectName}-${version}-${platform}.tar.gz`;
const prebuildFolder = path.join(__dirname, '../build/dist');

(async () => {
    // Create asset
    await tar.c({ gzip: true, file: assetsFileName }, [ prebuildFolder ]);

    // Create release
    let release;
    try {
        console.log(`owner=${owner}, repoName=${repoName}, version=${version}`);
        release = await promisify(gh.getByTag)(auth, owner, repoName, version);
    } catch (e) {
        release = await promisify(gh.create)(auth, owner, repoName, {
            tag_name: version,
            name: `${repoName}-${version}`,
            body: `${repoName}-${version} release`,
        });
    }

    // Upload asset file
    if (release.assets.some(a => a.name === assetsFileName)) {
        throw new Error(`${assetsFileName} already existed`);
    }
    await promisify(gh.uploadAssets)(auth, owner, repoName, `tags/${version}`, [ assetsFileName ]);

    // Delete tar file
    fs.unlinkSync(assetsFileName);
})();