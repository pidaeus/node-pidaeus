/*!
 * Attach chai to global should
 */

global.chai = require('chai');
global.should = global.chai.should();

/*!
 * Chai Plugins
 */

global.chai.use(require('chai-spies'));
//global.chai.use(require('chai-http'));

/*!
 * Import project
 */

global.pidaeus = require('../..');

/*!
 * Helper to load internals for cov unit tests
 */

function req (name) {
  return process.env.pidaeus_COV
    ? require('../../lib-cov/pidaeus/' + name)
    : require('../../lib/pidaeus/' + name);
}

/*!
 * Load unexposed modules for unit tests
 */

global.__pidaeus = {};
