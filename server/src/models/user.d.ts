import mongo = require("mongoose");
interface User {
    username: string;
    xp: number;
}
export declare const User: mongo.Model<User, {}, {}, {}, mongo.Document<unknown, {}, User, {}, mongo.DefaultSchemaOptions> & User & {
    _id: mongo.Types.ObjectId;
} & {
    __v: number;
} & {
    id: string;
}, any, User>;
export {};
//# sourceMappingURL=user.d.ts.map