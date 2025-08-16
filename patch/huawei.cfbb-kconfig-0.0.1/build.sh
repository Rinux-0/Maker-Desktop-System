echo "kConfig"

mkdir -p ../../../out/vsix

yarn config set strict-ssl false
yarn config set disable-self-update-check true
yarn
vsce package

cp ./*.vsix ../../../out/vsix