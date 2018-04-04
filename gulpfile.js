var gulp = require('gulp');
var git = require('gulp-git');
 
gulp.task('clone-libigl', function() {
    git.clone('https://github.com/libigl/libigl.git', {args: '--recursive'}, function (err) {
        if (err) throw err;
    });
});