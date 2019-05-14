if [ "$TRAVIS_PULL_REQUEST" != "false" ] ; then
	make clean -C benchmark
	make serial -C benchmark
	OUTPUT="$(./benchmark/benchmark.out | sed -E ':a;N;$!ba;s/\r{0,1}\n/\\n/g')"
	echo "{\"body\": \"$OUTPUT\"}"
	curl -H "Authorization: token ${GITHUB_TOKEN}" -X POST \
	-d "{\"body\": \"SERIAL:\n$OUTPUT\"}" \
	"https://api.github.com/repos/${TRAVIS_REPO_SLUG}/issues/${TRAVIS_PULL_REQUEST}/comments"
	make clean -C benchmark
	make parallel -C benchmark
	OUTPUT="$(./benchmark/benchmark.out | sed -E ':a;N;$!ba;s/\r{0,1}\n/\\n/g')"
	echo "{\"body\": \"$OUTPUT\"}"
	curl -H "Authorization: token ${GITHUB_TOKEN}" -X POST \
	-d "{\"body\": \"PARALLEL:\n$OUTPUT\"}" \
	"https://api.github.com/repos/${TRAVIS_REPO_SLUG}/issues/${TRAVIS_PULL_REQUEST}/comments"
fi
