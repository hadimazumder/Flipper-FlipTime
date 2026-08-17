# Release process

1. Update `fap_version` in `application.fam`.
2. Move relevant entries from `Unreleased` into a dated section in `CHANGELOG.md`.
3. Build against the Momentum release SDK:

   ```bash
   ufbt update --index-url https://up.momentum-fw.dev/firmware/directory.json --channel release
   ufbt
   ```

4. Test the generated FAP on a real Flipper Zero.
5. Commit the release changes.
6. Create an annotated tag:

   ```bash
   git tag -a v1.1.0 -m "Flip Time v1.1.0"
   git push origin v1.1.0
   ```

7. Create a GitHub Release from the tag.
8. Attach `dist/flip_time.fap`, naming the downloadable file `FlipTime.fap` if desired.
9. Copy the corresponding changelog section into the release notes.

Do not publish a FAP that has not passed `APPCHK` or that was compiled against an undocumented SDK source.
