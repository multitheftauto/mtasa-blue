const fs = require('fs').promises;

(async () => {
    const { ImagePool } = (await import('@squoosh/lib')).default;
    const { cpus } = (await import('os')).default;
    const imagePool = new ImagePool(cpus().length);

    const locales = (await fs.readdir('uncompressed', { withFileTypes: true }))
        .filter(dirent => dirent.isDirectory() && dirent.name[0] !== '.')
        .map(dirent => dirent.name);

    console.log('Doing', locales);

    await Promise.allSettled(locales.map(async (locale) => {
        console.log(`Doing ${locale}...`);

        //await fs.mkdir(`compressed/${locale}`, { recursive: true });

        const pngs = (await fs.readdir(`uncompressed/${locale}`, { withFileTypes: true }))
            .filter(dirent => dirent.isFile() && dirent.name[0] !== '.' && dirent.name.endsWith('.png'))
            .map(dirent => dirent.name);

        await Promise.allSettled(pngs.map(async (filename) => {
            const image = imagePool.ingestImage(`uncompressed/${locale}/${filename}`);

            try {
                await image.preprocess({ quant: { dither: 0.1, numColors: 8 } });
            } catch (e) {
                console.error('preprocess', `uncompressed/${locale}/${filename}`, e);
            }

            try {
                await image.encode({ oxipng: { level: 6 } });
            } catch (e) {
                console.error('encode', `uncompressed/${locale}/${filename}`, e);
            }

            //return fs.writeFile(`compressed/${locale}/${filename}`, (await image.encodedWith.oxipng).binary);
            return fs.writeFile(`../../../Shared/data/MTA San Andreas/MTA/locale/${locale}/${filename}`, (await image.encodedWith.oxipng).binary);
        }));

        console.log(`${locale} done!`);
    }));

    await imagePool.close();

    console.log('All done!');
})();
