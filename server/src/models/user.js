import { createRequire as _createRequire } from "module";
const __require = _createRequire(import.meta.url);
const mongo = __require("mongoose");
const mongoose = __require("mongoose");
;
const userSchema = new mongoose.Schema({
    username: { type: String, required: true, unique: true },
    xp: { type: Number, required: true, unique: false, default: 0 }
});
export const User = mongoose.model('User', userSchema);
//# sourceMappingURL=user.js.map