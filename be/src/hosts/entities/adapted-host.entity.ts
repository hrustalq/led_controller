import { Host } from "./host.entity";

export interface AdaptedHost extends Host {
    active: number;
}