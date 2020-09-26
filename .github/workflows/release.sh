#!/usr/bin/env bash
set -ex
cd "$(dirname -- "$(readlink -f -- "$0")")"
cd ../..

EDGE_TAG="edge"
EDGE_RELEASE_TITLE='Continuous build'
EDGE_RELEASE_NOTES='This is an automated build from the latest changes in the `master` branch. Stability is not guaranteed. Use at your own risk.'

GH_VER=1.0.0
curl -SsL "https://github.com/cli/cli/releases/download/v$GH_VER/gh_${GH_VER}_linux_amd64.tar.gz" \
    | tar -xzOf- "gh_${GH_VER}_linux_amd64/bin/gh" \
    | sudo tee "$(which gh)" > /dev/null

function getTagId
{
    TAG_NAME="$1"

    TAG_ID="$(
        gh api graphql -F owner=':owner' -F name=':repo' -F tagName="$TAG_NAME" -f query='
            query($name: String!, $owner: String!, $tagName: String!) {
                repository(owner: $owner, name: $name) {
                    ref (qualifiedName: $tagName) {
                        id
                    }
                }
            }
        ' | jq -r '.data.repository.ref.id'
    )"

    if [[ $TAG_ID == null ]]
    then
        echo "tag \"$TAG_NAME\" not found" >&2
        echo ""
    else
        echo -n "$TAG_ID"
    fi
}

function removeTagById
{
    TAG_ID="$1"

    RES="$(
        gh api graphql -F tagId="$TAG_ID" -F mutationId="abc" -f query='
            mutation($tagId: ID!, $mutationId: String!){
                deleteRef(input:{refId: $tagId, clientMutationId: $mutationId}) {
                    clientMutationId
                }
            }
        ' | jq -r '.data.deleteRef.clientMutationId'
    )"

    if [[ $RES == null ]]
    then
        echo "cannot remove tag id \"$TAG_ID\"" >&2
        exit 1
    fi
}

TAG_NAME="$1"
shift

FILES=(artifacts/*/*.tar.xz)

TAG_ID="$(getTagId "$TAG_NAME")"
if [[ $TAG_NAME == $EDGE_TAG ]]
then
    if [[ $TAG_ID ]]
    then
        gh release delete edge -y || true
        removeTagById "$TAG_ID"
    fi
    gh release create "$TAG_NAME" "${FILES[@]}" -p -t "$EDGE_RELEASE_TITLE" -n "$EDGE_RELEASE_NOTES"
else
    if [[ -z $TAG_ID ]]
    then
        exit 1
    fi
    RELEASE_DESC="$(grep -Pzom1 "(?s)\n$TAG_NAME .*?\K#.*?\n\n\n" CHANGELOG.md)"
    gh release create "$TAG_NAME" "${FILES[@]}" CHANGELOG.md README.md -t "$TAG_NAME" -n "$RELEASE_DESC"
fi
