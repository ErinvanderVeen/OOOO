if [ "$TRAVIS_PULL_REQUEST" != "false" ] ; then
	make clean -C ai
	make metrics -C ai
	OUTPUT="$(./src/othello.out | sed -E ':a;N;$!ba;s/\r{0,1}\n/\\n/g')"
	echo "{\"body\": \"$OUTPUT\"}"
	curl -H "Authorization: token ${GITHUB_TOKEN}" -X POST \
	-d "{\"body\": \"$OUTPUT\"}" \
	"https://api.github.com/repos/${TRAVIS_REPO_SLUG}/issues/${TRAVIS_PULL_REQUEST}/comments"
fi
