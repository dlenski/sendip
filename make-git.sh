links=$(cat <<EOF
https://www-x.antd.nist.gov/ipv6/sendip/sendip-2.5-mec-0.tar.gz
https://www-x.antd.nist.gov/ipv6/sendip/sendip-2.5-mec-1a4.tar.gz
https://www-x.antd.nist.gov/ipv6/sendip/sendip-2.5-mec-2a2.tar.gz
https://www-x.antd.nist.gov/ipv6/sendip/sendip-2.5-mec-3a1.tar.gz
https://www-x.antd.nist.gov/ipv6/sendip/sendip-2.5-mec-3a2.tar.gz
EOF
)

echo "make-git.sh" > .gitignore

for link in $links
do
	echo "=========================="
	echo "$link"
	echo "=========================="
	version=$(echo "$link" | cut -f6 -d/)
	curl "$link" | tar -xvz --strip-components=1

	git add .
	git commit --author "Mike Ricketts <mike@earth.li>" -m "$version"
	git rm -r --cached 2> /dev/null
done

git reset --hard
