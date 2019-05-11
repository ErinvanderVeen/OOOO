if [ "$TRAVIS_PULL_REQUEST" != "false" ] ; then
	make clean -C src
	make metrics -C src
	OUTPUT="$(./src/othello.out)"
	curl -H "Authorization: token ${GITHUB_TOKEN}" -X POST \
	-d "{\"body\": \"$OUTPUT\"}" \
	"https://api.github.com/repos/${TRAVIS_REPO_SLUG}/issues/${TRAVIS_PULL_REQUEST}/comments"
fi
