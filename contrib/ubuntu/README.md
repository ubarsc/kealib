# To build .deb packages

Run the following command **from the root of the repository**

`docker build . -f contrib/ubuntu/focal_ubuntugis_stable/Dockerfile -t kealib_deb_builder`

This creates a docker image which should have the debian built in /usr/local/packages from where you can copy it out, e.g.,

`docker run --rm --entrypoint cat kealib_deb_builder /usr/local/packages/libkea_1.4.14-1.deb > /tmp/libkea_1.4.14-1.deb`
