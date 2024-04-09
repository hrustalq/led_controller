import { Injectable } from '@nestjs/common';
import { HttpService } from '@nestjs/axios';
import { Cron, CronExpression } from '@nestjs/schedule';
import { ConfigService } from '@nestjs/config';
import { firstValueFrom } from 'rxjs';
import { Host } from './entities/host.entity';
import { GetHostsResponseDto } from './dto/get-hosts.dto';
import { Session } from './entities/session.entity';
import { GetSessionsResponseDto } from './dto/get-sessions.dto';

@Injectable()
export class HostsService {
  constructor(
    private httpService: HttpService,
    private configService: ConfigService,
  ) {}

  // setup for external api request (gizmo)
  private GIZMO_API_URL = this.configService.get<string>('GIZMO_API_HOST');
  private GIZMO_OPERATOR_CREDENTIALS = this.configService.get<string>(
    'GIZMO_API_OPERATOR_CREDENTIALS',
  );

  public activeHosts: Host[] = [];
  public hosts: Host[] = [];
  public activeSessions: Session[] = [];

  @Cron(CronExpression.EVERY_SECOND)
  filterHostsToActive() {
    this.activeSessions.forEach((value, idx) => {
      if (this.hosts[idx].number === value.hostNumber) {
        this.activeHosts.push(this.hosts[idx]);
      }
    });
  }

  @Cron(CronExpression.EVERY_SECOND)
  async getActiveUserSessions() {
    const REQUEST_URL = `${this.GIZMO_API_URL}/api/usersessions/activeinfo`;
    const response = await firstValueFrom(
      this.httpService.get<GetSessionsResponseDto>(REQUEST_URL, {
        headers: {
          Authorization: `Basic ${this.GIZMO_OPERATOR_CREDENTIALS}`,
        },
      }),
    ).then((response) => response.data);
    if (!response.result.length || response.result === this.activeSessions)
      return;
    this.activeSessions = [...response.result];
  }
  @Cron(CronExpression.EVERY_HOUR)
  async getAllHosts() {
    const REQUEST_URL = `${this.GIZMO_API_URL}/api/hosts`;
    const response = await firstValueFrom(
      this.httpService.get<GetHostsResponseDto>(REQUEST_URL, {
        headers: {
          Authorization: `Basic ${this.GIZMO_OPERATOR_CREDENTIALS}`,
        },
      }),
    )
      .then((response) => response.data)
      .catch((error) => {
        console.log(error);
      });
    if (response && response.result.length) {
      const hosts = response.result.filter((host) => !host.isDeleted);
      if (!hosts.length || hosts === this.hosts) return;
      this.hosts = [...hosts].sort((a, b) => a.number - b.number);
    } else return;
  }

  findAll() {
    return this.hosts;
  }

  findOne(id: number) {
    return `This action returns a #${id} host`;
  }
}
