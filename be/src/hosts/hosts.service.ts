import { Injectable, NotFoundException } from '@nestjs/common';
import { HttpService } from '@nestjs/axios';
import { Cron, CronExpression } from '@nestjs/schedule';
import { ConfigService } from '@nestjs/config';
import { firstValueFrom } from 'rxjs';
import { Host } from './entities/host.entity';
import { GetHostsResponseDto } from './dto/get-hosts.dto';
import { Session } from './entities/session.entity';
import { GetSessionsResponseDto } from './dto/get-sessions.dto';
import { AdaptedHost } from './entities/adapted-host.entity';

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

  public hosts = new Map<number, AdaptedHost>();
  public activeSessions = new Map<number, Session>()

  @Cron(CronExpression.EVERY_5_SECONDS)
  filterHostsToActive() {
    this.hosts.forEach((host) => {
      if (this.activeSessions.has(host.number)) {
        host.active = 1;
      } else {
        host.active = 0;
      }
    });
  }

  @Cron(CronExpression.EVERY_5_SECONDS)
  async getActiveUserSessions() {
    const REQUEST_URL = `${this.GIZMO_API_URL}/api/usersessions/activeinfo`;
    const response = await firstValueFrom(
      this.httpService.get<GetSessionsResponseDto>(REQUEST_URL, {
        headers: {
          Authorization: `Basic ${this.GIZMO_OPERATOR_CREDENTIALS}`,
        },
      }),
    ).then((response) => response.data);
    if (!response.result.length) {
      this.activeSessions.clear();
    };
    response.result.forEach((activeSession) => {
      this.activeSessions.set(activeSession.hostNumber, activeSession);
    });
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
      response.result.forEach((host) => {
        if (!host.isDeleted) {
          this.hosts.set(host.number, {...host, active: 0})
        }
      });
    } else return;
  }

  findAll() {
    return this.hosts;
  }

  findActive(id: number) {
    if (this.hosts.has(id)) {
      return this.hosts.get(id).active
    } else return;
  }
}
